#include "satellite.h"

#include "hashmap.h"
#include "satcat_code.h"
#include <cglm/mat3.h>
#include <cglm/mat4.h>
#include <cglm/project.h>
#include <cglm/vec3.h>
#include <curl/curl.h>
#include <gmodule.h>

#include <string.h>

#include "bo.h"
#include "camera.h"
#include "catalog.h"
#include "download.h"
#include "error.h"
#include "gfx.h"
#include "mem.h"
#include "phys.h"
#include "shader.h"
#include "status.h"
#include "thread.h"
#include "vao.h"

enum LayoutLoc {
	LOCL_APOS = 0u,
	LOCL_COLOR,
};

enum UniformLoc {
	LOCU_TRANSFORM = 0u,
};

enum DLMultiLoc {
	DL_SATCAT = 0u,
	DL_TLE,
};

static struct DLMulti dl_multi;

static struct VAO vao_satellites;
static struct BO vbo_verts;
static struct BO vbo_vert_colors;

static struct VAO vao_orbits;
static struct BO vbo_orbits;
static struct BO vbo_orbit_colors;

static struct Shader shader;

static mat4 teme_to_world;
static mat4 transform;

static size_t n_satellites_sync = 0;
static struct Satellite *satellites_sync = NULL;
static vec3 *vert_colors_sync = NULL;

static struct Satellite *satellites = NULL;
static size_t n_satellites = 0;
static bool satellites_renderable = false;
static vec3 *satellite_verts = NULL;

static size_t n_satellite_orbits = 0;
static size_t satellite_orbits_size = 0;
static GArray *satellite_orbit_firsts = NULL;
static GArray *satellite_orbit_counts = NULL;
static vec3 *satellite_orbits = NULL;
static GArray *satellite_orbit_idxs = NULL;
static vec3 *satellite_orbit_colors = NULL;

static void satellite_toggle_orbit(uint32_t idx);
static void satellites_get(void);
static uint64_t sc_hash(const void *item, uint64_t seed0, uint64_t seed1);
static int sc_compare(const void *a, const void *b, void *udata);
static void set_satellite_color(char code, vec3 color);
static void save_satellite_cache(void);
static void load_cache(struct DLHandle *handle, uint64_t size, FILE *cache);

void satellite_init(void)
{
	const char *urls[] = { "https://celestrak.org/pub/satcat.txt", "https://celestrak.org/pub/TLE/catalog.txt" };
	dl_multi_init(&dl_multi, 2, urls);

	glm_mat4_identity(teme_to_world);

	vao_init(&vao_satellites);
	vao_bind(&vao_satellites);
	bo_init(&vbo_verts, GL_ARRAY_BUFFER, true);
	bo_init(&vbo_vert_colors, GL_ARRAY_BUFFER, true);
	vao_attr(&vao_satellites, &vbo_verts, LOCL_APOS, 3, GL_FLOAT, sizeof(vec3), 0);
	vao_attr(&vao_satellites, &vbo_vert_colors, LOCL_COLOR, 3, GL_FLOAT, sizeof(vec3), 0);

	vao_init(&vao_orbits);
	vao_bind(&vao_orbits);
	bo_init(&vbo_orbits, GL_ARRAY_BUFFER, true);
	bo_init(&vbo_orbit_colors, GL_ARRAY_BUFFER, true);
	vao_attr(&vao_orbits, &vbo_orbits, LOCL_APOS, 3, GL_FLOAT, sizeof(vec3), 0);
	vao_attr(&vao_orbits, &vbo_orbit_colors, LOCL_COLOR, 3, GL_FLOAT, sizeof(vec3), 0);

	shader_init(&shader, "res/shader/satellite.vert", "res/shader/satellite.frag", 2, (struct ShaderAttr[]){
												  { LOCL_APOS, "in_pos" },
												  { LOCL_COLOR, "in_color" },
											  });

	satellite_orbit_firsts = g_array_new(FALSE, FALSE, sizeof(GLint));
	satellite_orbit_counts = g_array_new(FALSE, FALSE, sizeof(GLsizei));
	satellite_orbit_idxs = g_array_new(FALSE, FALSE, sizeof(uint32_t));
}

void satellite_deinit(void)
{
	dl_multi_deinit(&dl_multi);

	vao_deinit(&vao_satellites);
	bo_deinit(&vbo_verts);
	bo_deinit(&vbo_vert_colors);

	vao_deinit(&vao_orbits);
	bo_deinit(&vbo_orbits);
	bo_deinit(&vbo_orbit_colors);

	shader_deinit(&shader);

	free(satellites);
	free(satellite_verts);
	free(satellite_orbits);
	free(satellite_orbit_colors);
	g_array_unref(satellite_orbit_firsts);
	g_array_unref(satellite_orbit_counts);
	g_array_unref(satellite_orbit_idxs);
}

void satellites_get_prep(void)
{
	status_push(STAT_FETCHING_SAT, "Fetching satellite data...");
}

void *satellites_get_thrd(void *arguments)
{
	struct Thread *thread = (struct Thread *)arguments;
	satellites_get();
	g_atomic_int_set(&thread->finished, 1);
	return NULL;
}

static uint64_t sc_hash(const void *item, uint64_t seed0, uint64_t seed1)
{
	(void)seed0;
	(void)seed1;
	return ((struct SatCat *)item)->catnum;
}

static int sc_compare(const void *a, const void *b, void *udata)
{
	(void)udata;
	return (int)(((struct SatCat *)a)->catnum) - (int)(((struct SatCat *)b)->catnum);
}

void save_satellite_cache(void)
{
	FILE *cache = fopen(".sat_cache", "w");
	long epoch_ms = system_epoch_ms();
	fwrite(&epoch_ms, 8, 1, cache);
	fwrite(&dl_multi.handles[DL_SATCAT].size, 8, 1, cache);
	fwrite(&dl_multi.handles[DL_TLE].size, 8, 1, cache);
	fwrite(dl_multi.handles[DL_SATCAT].memory, 1, dl_multi.handles[DL_SATCAT].size, cache);
	fwrite(dl_multi.handles[DL_TLE].memory, 1, dl_multi.handles[DL_TLE].size, cache);
	fclose(cache);
}

void load_cache(struct DLHandle *handle, uint64_t size, FILE *cache)
{
	handle->size = size;
	handle->memory = safe_malloc(size);
	fread(handle->memory, 1, size, cache);
}

void satellites_get(void)
{
	FILE *cache = fopen(".sat_cache", "r");
	if (!cache) {
		dl_multi_perform(&dl_multi);
		save_satellite_cache();
	} else {
		long cache_time;
		fread(&cache_time, 8, 1, cache);
		if (system_epoch_ms() - cache_time > 43200000L) {
			fclose(cache);
			dl_multi_perform(&dl_multi);
			save_satellite_cache();
		} else {
			uint64_t sizes[2];
			fread(sizes, 8, 2, cache);
			load_cache(&dl_multi.handles[DL_SATCAT], sizes[0], cache);
			load_cache(&dl_multi.handles[DL_TLE], sizes[1], cache);
			fclose(cache);
		}
	}

	struct DLHandle *dl_satcat = &dl_multi.handles[DL_SATCAT];
	size_t n_satcat = count((char *)dl_satcat->memory, '\n', dl_satcat->size);
	struct hashmap *map = hashmap_new(sizeof(struct SatCat), 4 * n_satcat / 3 + 1, 0, 0, &sc_hash, &sc_compare, NULL, NULL);
	char *c = (char *)dl_satcat->memory;
	size_t i;
	for (i = 0; i < n_satcat; i++) {
		if (sc_validate(c)) {
			struct SatCat sc;
			sc_parse(&sc, c);
			if (sc.opstat != SCSTAT_DECAYED)
				hashmap_set(map, &sc);
		}
		c += 134;
	}

	free(dl_satcat->memory);
	dl_satcat->memory = NULL;

	struct DLHandle *dl_tle = &dl_multi.handles[DL_TLE];
	size_t ln_cnt = count((char *)dl_tle->memory, '\n', dl_tle->size);
	size_t max_satellites = ln_cnt / 3;
	n_satellites_sync = 0;
	satellites_sync = safe_malloc(sizeof(struct Satellite) * max_satellites);

	c = (char *)dl_tle->memory;
	for (i = 0; i < max_satellites; i++) {
		struct Satellite *sat = &satellites_sync[n_satellites_sync];
		parseLines(&sat->tle, c + 26, c + 97);
		struct SatCat *sc = hashmap_get(map, &(struct SatCat){ .catnum = sat->tle.objectNum });
		if (sc) {
			memcpy(sat->name, c, 24);
			sat->name[24] = '\0';
			sat->orbit_idx = UINT32_MAX;
			memcpy(&sat->satcat, sc, sizeof(struct SatCat));
			n_satellites_sync++;
		}
		c += 168;
	}

	if (n_satellites_sync != max_satellites)
		satellites_sync = safe_realloc(satellites_sync, sizeof(struct Satellite) * n_satellites_sync);

	hashmap_free(map);
	free(dl_tle->memory);
	dl_tle->memory = NULL;

	vert_colors_sync = safe_malloc(sizeof(vec3) * n_satellites_sync);
	for (i = 0; i < n_satellites_sync; i++)
		set_satellite_color(satellites_sync[i].satcat.opstat, vert_colors_sync[i]);
}

static void set_satellite_color(char code, vec3 color)
{
	switch (code) {
	case SCSTAT_OPERATIONAL:
	case SCSTAT_PARTIALLY_OPERATIONAL:
	case SCSTAT_BACKUP:
	case SCSTAT_SPARE:
	case SCSTAT_EXTENDED_MISSION:
		glm_vec3_copy((vec3){ 0.f, 0.9f, 0.f }, color);
		break;
	case SCSTAT_NONOPERATIONAL:
		glm_vec3_copy((vec3){ 0.9f, 0.0f, 0.f }, color);
		break;
	/*case SCSTAT_UNKNOWN:
	case ' ':*/
	default:
		glm_vec3_copy((vec3){ 0.6f, 0.6f, 0.6f }, color);
	}
}

void satellites_get_sync(void)
{
	free(satellites);
	satellites = satellites_sync;
	n_satellites = n_satellites_sync;
	free(satellite_verts);
	satellite_verts = safe_malloc(sizeof(vec3) * n_satellites);
	bo_buffer(&vbo_vert_colors, vert_colors_sync, sizeof(vec3) * n_satellites);
	free(vert_colors_sync);
	satellites_renderable = false;
	status_pop(STAT_FETCHING_SAT);
	catalog_satellites_fill(satellites, n_satellites);
}

void satellites_phys_sync(void)
{
	if (!n_satellites)
		return;

	bo_buffer(&vbo_verts, satellite_verts, sizeof(vec3) * n_satellites);
	satellites_renderable = true;
}

void satellites_phys(void)
{
	if (!n_satellites)
		return;

	mat3 t;
	glm_vec3_copy((vec3){ 0.f, 0.f, 1.f }, t[2]);
	glm_vec3_copy((vec3){ (float)cos(g_phys.gmst), (float)-sin(g_phys.gmst), 0.f }, t[0]);
	glm_vec3_cross(t[2], t[0], t[1]);
	glm_vec3_norm(t[1]);
	glm_mat3_scale(t, 1.f / 6371.f);
	glm_mat4_ins3(t, teme_to_world);

	size_t i;
	for (i = 0; i < n_satellites; i++) {
		double r[3], v[3];
		getRVForDate(&satellites[i].tle, g_phys.epoch_ms, r, v);
		glm_vec3_copy((vec3){ r[0], r[1], r[2] }, satellite_verts[i]);
	}
}

void satellites_render(void)
{
	shader_bind(&shader);
	camera_mvp_generate(&g_camera, &teme_to_world, transform);
	glUniformMatrix4fv(LOCU_TRANSFORM, 1, GL_FALSE, (const GLfloat *)&transform);

	if (satellites_renderable) {
		vao_bind(&vao_satellites);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glDrawArrays(GL_POINTS, 0, n_satellites);
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

		vao_bind(&vao_orbits);
		glMultiDrawArrays(GL_LINE_LOOP, (GLint *)satellite_orbit_firsts->data, (GLint *)satellite_orbit_counts->data, n_satellite_orbits);
	}
}

static void satellite_toggle_orbit(uint32_t idx)
{
	if (satellites[idx].orbit_idx == UINT32_MAX) { /* Enable orbit */
		/* For similar-length line segments, apply Kepler's Third Law (T^2 prop. r^3) */
		int count = 250.f / cbrtf(satellites[idx].tle.n * satellites[idx].tle.n);

		satellites[idx].orbit_idx = n_satellite_orbits;

		g_array_append_val(satellite_orbit_idxs, idx);
		g_array_append_val(satellite_orbit_counts, count);
		GLint first = n_satellite_orbits ? (g_array_index(satellite_orbit_firsts, GLint, n_satellite_orbits - 1) + g_array_index(satellite_orbit_counts, GLsizei, n_satellite_orbits - 1)) : 0;
		g_array_append_val(satellite_orbit_firsts, first);

		satellite_orbits_size += count * sizeof(vec3);
		satellite_orbits = safe_realloc(satellite_orbits, satellite_orbits_size);
		satellite_orbit_colors = safe_realloc(satellite_orbit_colors, satellite_orbits_size);

		long period = MS_IN_DAY / (float)satellites[idx].tle.n;
		int i;
		for (i = 0; i < count; i++) {
			long epoch_ms = g_phys.epoch_ms + period * i / count;
			double r[3], v[3];
			getRVForDate(&satellites[idx].tle, epoch_ms, r, v);
			glm_vec3_copy((vec3){ r[0], r[1], r[2] }, satellite_orbits[first + i]);
			set_satellite_color(satellites[idx].satcat.opstat, satellite_orbit_colors[first + i]);
		}

		bo_buffer(&vbo_orbits, satellite_orbits, satellite_orbits_size);
		bo_buffer(&vbo_orbit_colors, satellite_orbit_colors, satellite_orbits_size);

		n_satellite_orbits++;
	} else { /* Disable orbit */
		uint32_t orbit_idx = satellites[idx].orbit_idx;
		satellites[idx].orbit_idx = UINT32_MAX;

		size_t count = g_array_index(satellite_orbit_counts, GLsizei, orbit_idx);

		size_t count_move = 0;
		uint32_t i;
		for (i = orbit_idx + 1; i < n_satellite_orbits; i++) {
			g_array_index(satellite_orbit_firsts, GLsizei, i) -= count;
			satellites[g_array_index(satellite_orbit_idxs, uint32_t, i)].orbit_idx--;
			count_move += g_array_index(satellite_orbit_counts, GLsizei, i);
		}

		size_t size_move = count_move * sizeof(vec3);
		size_t size_remove = g_array_index(satellite_orbit_counts, GLsizei, orbit_idx) * sizeof(vec3);
		memmove(((uint8_t *)satellite_orbits) + (satellite_orbits_size - size_move - size_remove), ((uint8_t *)satellite_orbits) + (satellite_orbits_size - size_move), size_move);
		memmove(((uint8_t *)satellite_orbit_colors) + (satellite_orbits_size - size_move - size_remove), ((uint8_t *)satellite_orbit_colors) + (satellite_orbits_size - size_move), size_move);
		satellite_orbits_size -= size_remove;
		bo_buffer(&vbo_orbits, satellite_orbits, satellite_orbits_size);
		bo_buffer(&vbo_orbit_colors, satellite_orbit_colors, satellite_orbits_size);

		g_array_remove_index(satellite_orbit_firsts, orbit_idx);
		g_array_remove_index(satellite_orbit_counts, orbit_idx);
		g_array_remove_index(satellite_orbit_idxs, orbit_idx);

		n_satellite_orbits--;
	}
}

void satellite_select(double xpos, double ypos)
{
	vec3 p1, p2;
	glm_unproject((vec3){ xpos, g_gl_ctx.res_y - ypos, 0 }, transform, (vec4){ 0, 0, g_gl_ctx.res_x, g_gl_ctx.res_y }, p1);
	glm_unproject((vec3){ xpos, g_gl_ctx.res_y - ypos, 1 }, transform, (vec4){ 0, 0, g_gl_ctx.res_x, g_gl_ctx.res_y }, p2);

	vec3 dir;
	glm_vec3_sub(p2, p1, dir);
	float dir_inv_len2 = 1.f / glm_vec3_norm2(dir);

	float min_dist2 = FLT_MAX;
	uint32_t idx;

	size_t i;
	for (i = 0; i < n_satellites; i++) {
		vec3 p1sat;
		glm_vec3_sub(satellite_verts[i], p1, p1sat);
		float dp = glm_vec3_dot(dir, p1sat);
		float dist2 = glm_vec3_norm2(p1sat);
		float cos_ang2 = (dp * dp * dir_inv_len2) / dist2;
		if (unlikely(cos_ang2 > 0.9994f && dp > 0 && dist2 < min_dist2)) {
			idx = i;
			min_dist2 = dist2;
		}
	}

	if (min_dist2 != FLT_MAX) {
		satellite_toggle_orbit(idx);
		catalog_satellite_changed(&satellites[idx]);
	}
}

void satellite_select_ptr(struct Satellite *satellite)
{
	satellite_toggle_orbit(satellite - satellites);
}
