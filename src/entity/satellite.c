/*
 * Copyright (c) 2022-2023 Wojciech Graj
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 **/

#include "satellite.h"

#include "hashmap.h"
#include "satcat_code.h"
#include <cglm/mat3.h>
#include <cglm/mat4.h>
#include <cglm/project.h>
#include <cglm/vec3.h>
#include <curl/curl.h>
#include <glib/gstdio.h>
#include <gmodule.h>

#include "bo.h"
#include "camera.h"
#include "catalog.h"
#include "download.h"
#include "error.h"
#include "filter.h"
#include "gfx.h"
#include "perf.h"
#include "phys.h"
#include "shader.h"
#include "status.h"
#include "thread.h"
#include "vao.h"

#define SATELLITE_COLOR_ACTIVE ((vec3){ 0.f, 0.9f, 0.f })
#define SATELLITE_COLOR_NONOPERATIONAL ((vec3){ 0.9f, 0.0f, 0.f })
#define SATELLITE_COLOR_OTHER ((vec3){ 0.6f, 0.6f, 0.6f })
#define ORBIT_SEGMENT_LENGTH 250.f
#define ORBIT_MAX_DELTA_DIV 4L
#define SATELLITE_SELECT_MIN_COS_ANG2 0.9995f
#define WAIT_USEC 100UL
#define SATELLITE_CALC_STEP 2000
#define MAX_THREAD_TIC 2
#define CACHE_INVAL_MS 43200000LL

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

struct OrbitData {
	guint32 idx;
	gint64 start_epoch_ms;
	gint64 max_delta_ms;
};

extern const unsigned char res_shader_satellite_vert[];
extern const unsigned int res_shader_satellite_vert_len;
extern const unsigned char res_shader_satellite_frag[];
extern const unsigned int res_shader_satellite_frag_len;

static struct DLMulti dl_multi;

static struct VAO vao_satellites;
static struct BO vbo_verts;
static struct BO vbo_vert_colors;

static struct VAO vao_orbits;
static struct BO vbo_orbits;
static struct BO vbo_orbit_colors;

static struct Shader shader;

static mat4 transform;

static size_t n_satellites_sync = 0;
static struct Satellite *satellites_sync = NULL;
static vec3 *vert_colors_sync = NULL;

static struct Satellite *satellites = NULL;
static size_t n_satellites = 0;
static gboolean satellites_renderable = FALSE;
static vec3 *satellite_verts = NULL;
static size_t n_satellites_render = 0;
static GLuint *satellite_indices = NULL;

static size_t n_satellite_orbits = 0;
static size_t satellite_orbits_size = 0;
static GArray *satellite_orbit_firsts = NULL;
static GArray *satellite_orbit_counts = NULL;
static vec3 *satellite_orbits = NULL;
static GArray *satellite_orbit_data = NULL;
static vec3 *satellite_orbit_colors = NULL;
static gboolean orbits_changed_phys = FALSE;

static gboolean recache = FALSE;

static const char *CACHE_FILENAME = ".sat_cache";

static GThreadPool *thread_pool;
static guint32 *thread_pool_indices_sync;
static guint32 *thread_pool_indices = NULL;

static void satellite_toggle_orbit(guint32 idx);
static void gen_satellite_orbit(guint32 idx);
static void satellites_get(void);
static void alloc_orbit_arrays(void);
static void dealloc_orbit_arrays(void);
static guint64 sc_hash(const void *item, guint64 seed0, guint64 seed1);
static int sc_compare(const void *a, const void *b, void *udata);
static void set_satellite_color(char code, vec3 color);
static void save_satellite_cache(void);
static void load_cache(struct DLHandle *handle, guint64 size, FILE *cache);
static void satellites_calc_pos(gpointer data, gpointer user_data);

void satellite_init(void)
{
	const char *urls[] = { "https://celestrak.org/pub/satcat.txt", "https://celestrak.org/pub/TLE/catalog.txt" };
	dl_multi_init(&dl_multi, 2, urls);

	vao_init(&vao_satellites);
	vao_bind(&vao_satellites);
	bo_init(&vbo_verts, GL_ARRAY_BUFFER, TRUE);
	bo_init(&vbo_vert_colors, GL_ARRAY_BUFFER, TRUE);
	vao_attr(&vao_satellites, &vbo_verts, LOCL_APOS, 3, GL_FLOAT, sizeof(vec3), 0);
	vao_attr(&vao_satellites, &vbo_vert_colors, LOCL_COLOR, 3, GL_FLOAT, sizeof(vec3), 0);

	vao_init(&vao_orbits);
	vao_bind(&vao_orbits);
	bo_init(&vbo_orbits, GL_ARRAY_BUFFER, TRUE);
	bo_init(&vbo_orbit_colors, GL_ARRAY_BUFFER, TRUE);
	vao_attr(&vao_orbits, &vbo_orbits, LOCL_APOS, 3, GL_FLOAT, sizeof(vec3), 0);
	vao_attr(&vao_orbits, &vbo_orbit_colors, LOCL_COLOR, 3, GL_FLOAT, sizeof(vec3), 0);

	shader_init(&shader, res_shader_satellite_vert, res_shader_satellite_vert_len, res_shader_satellite_frag, res_shader_satellite_frag_len, 2, (struct ShaderAttr[]){
																			    { LOCL_APOS, "in_pos" },
																			    { LOCL_COLOR, "in_color" },
																		    });

	alloc_orbit_arrays();

	guint num_proc = g_get_num_processors();
	thread_pool = g_thread_pool_new(satellites_calc_pos, NULL, MIN(num_proc, MAX_THREAD_TIC), TRUE, NULL);
}

void alloc_orbit_arrays(void)
{
	satellite_orbit_firsts = g_array_new(FALSE, FALSE, sizeof(GLint));
	satellite_orbit_counts = g_array_new(FALSE, FALSE, sizeof(GLsizei));
	satellite_orbit_data = g_array_new(FALSE, FALSE, sizeof(struct OrbitData));
}

void dealloc_orbit_arrays(void)
{
	g_array_unref(satellite_orbit_firsts);
	g_array_unref(satellite_orbit_counts);
	g_array_unref(satellite_orbit_data);
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

	g_free(satellites);
	g_free(satellite_verts);
	g_free(satellite_orbits);
	g_free(satellite_orbit_colors);

	dealloc_orbit_arrays();

	g_thread_pool_free(thread_pool, TRUE, FALSE);
}

void satellites_filter(void)
{
	n_satellites_render = 0;
	size_t i;
	for (i = 0; i < n_satellites; i++) {
		if (filter_func(&satellites[i])) {
			satellite_indices[n_satellites_render] = i;
			n_satellites_render++;
		} else if (satellites[i].orbit_idx != UINT32_MAX)
			satellite_select_ptr(&satellites[i]);
	}
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

guint64 sc_hash(const void *item, guint64 seed0, guint64 seed1)
{
	(void)seed0;
	(void)seed1;
	return ((struct SatCat *)item)->catnum;
}

int sc_compare(const void *a, const void *b, void *udata)
{
	(void)udata;
	return (int)(((struct SatCat *)a)->catnum) - (int)(((struct SatCat *)b)->catnum);
}

void satellite_clear_cache(void)
{
	g_remove(CACHE_FILENAME);
	recache = TRUE;
}

void save_satellite_cache(void)
{
	FILE *cache = fopen(CACHE_FILENAME, "w");
	gint64 epoch_ms = system_epoch_ms();
	fwrite(&epoch_ms, 8, 1, cache);
	fwrite(&dl_multi.handles[DL_SATCAT].size, 8, 1, cache);
	fwrite(&dl_multi.handles[DL_TLE].size, 8, 1, cache);
	fwrite(dl_multi.handles[DL_SATCAT].memory, 1, dl_multi.handles[DL_SATCAT].size, cache);
	fwrite(dl_multi.handles[DL_TLE].memory, 1, dl_multi.handles[DL_TLE].size, cache);
	fclose(cache);
}

void load_cache(struct DLHandle *handle, guint64 size, FILE *cache)
{
	handle->size = size;
	handle->memory = g_malloc(size);
	fread(handle->memory, 1, size, cache);
}

void satellites_get(void)
{
	if (recache) {
		recache = FALSE;
		dl_multi_perform(&dl_multi);
		save_satellite_cache();
	} else {
		FILE *cache = fopen(CACHE_FILENAME, "r");
		if (!cache) {
			dl_multi_perform(&dl_multi);
			save_satellite_cache();
		} else {
			gint64 cache_time;
			fread(&cache_time, 8, 1, cache);
			if (system_epoch_ms() - cache_time > CACHE_INVAL_MS) {
				fclose(cache);
				dl_multi_perform(&dl_multi);
				save_satellite_cache();
			} else {
				guint64 sizes[2];
				fread(sizes, 8, 2, cache);
				load_cache(&dl_multi.handles[DL_SATCAT], sizes[0], cache);
				load_cache(&dl_multi.handles[DL_TLE], sizes[1], cache);
				fclose(cache);
			}
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

	g_free(dl_satcat->memory);
	dl_satcat->memory = NULL;

	struct DLHandle *dl_tle = &dl_multi.handles[DL_TLE];
	size_t ln_cnt = count((char *)dl_tle->memory, '\n', dl_tle->size);
	size_t max_satellites = ln_cnt / 3;
	n_satellites_sync = 0;
	satellites_sync = g_malloc(sizeof(struct Satellite) * max_satellites);

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
		satellites_sync = g_realloc(satellites_sync, sizeof(struct Satellite) * n_satellites_sync);

	hashmap_free(map);
	g_free(dl_tle->memory);
	dl_tle->memory = NULL;

	vert_colors_sync = g_malloc(sizeof(vec3) * n_satellites_sync);
	for (i = 0; i < n_satellites_sync; i++)
		set_satellite_color(satellites_sync[i].satcat.opstat, vert_colors_sync[i]);

	guint32 n_thread_pool_indices = (SATELLITE_CALC_STEP - 1 + n_satellites_sync) / SATELLITE_CALC_STEP;
	thread_pool_indices_sync = g_malloc(sizeof(guint32) * n_thread_pool_indices);
	for (i = 0; i < n_thread_pool_indices; i++)
		thread_pool_indices_sync[i] = SATELLITE_CALC_STEP * i;
}

void set_satellite_color(char code, vec3 color)
{
	switch (code) {
	case SCSTAT_OPERATIONAL:
	case SCSTAT_PARTIALLY_OPERATIONAL:
	case SCSTAT_BACKUP:
	case SCSTAT_SPARE:
	case SCSTAT_EXTENDED_MISSION:
		glm_vec3_copy(SATELLITE_COLOR_ACTIVE, color);
		break;
	case SCSTAT_NONOPERATIONAL:
		glm_vec3_copy(SATELLITE_COLOR_NONOPERATIONAL, color);
		break;
	/*case SCSTAT_UNKNOWN:
	case ' ':*/
	default:
		glm_vec3_copy(SATELLITE_COLOR_OTHER, color);
	}
}

void satellites_get_sync(void)
{
	satellites_renderable = FALSE;
	g_free(satellites);
	satellites = satellites_sync;
	n_satellites = n_satellites_sync;
	g_free(satellite_verts);
	satellite_verts = g_malloc(sizeof(vec3) * n_satellites);
	bo_buffer(&vbo_vert_colors, vert_colors_sync, sizeof(vec3) * n_satellites);
	g_free(vert_colors_sync);

	g_free(satellite_indices);
	satellite_indices = g_malloc(sizeof(GLuint) * n_satellites);

	n_satellite_orbits = 0;
	satellite_orbits_size = 0;
	dealloc_orbit_arrays();
	alloc_orbit_arrays();
	g_free(satellite_orbits);
	satellite_orbits = NULL;
	g_free(satellite_orbit_colors);
	satellite_orbit_colors = NULL;

	g_free(thread_pool_indices);
	thread_pool_indices = thread_pool_indices_sync;

	catalog_satellites_fill(satellites, n_satellites);
	perf_set_num_satellites(n_satellites);
	satellites_filter();

	status_pop(STAT_FETCHING_SAT);
}

void satellites_tic_sync(void)
{
	if (!n_satellites)
		return;

	while (g_thread_pool_unprocessed(thread_pool))
		g_usleep(WAIT_USEC);

	bo_buffer(&vbo_verts, satellite_verts, sizeof(vec3) * n_satellites);
	satellites_renderable = TRUE;
}

void satellites_tic(void)
{
	if (!n_satellites)
		return;

	guint32 n_indices = (SATELLITE_CALC_STEP - 1 + n_satellites_render) / SATELLITE_CALC_STEP;

	guint32 i;
	for (i = 0; i < n_indices; i++)
		g_thread_pool_push(thread_pool, &thread_pool_indices[i], NULL);
}

void satellites_calc_pos(gpointer data, gpointer user_data)
{
	(void)user_data;
	guint32 start = *((guint32 *)data);
	guint32 end = MIN(n_satellites_render, start + SATELLITE_CALC_STEP);
	guint32 i;
	for (i = start; i < end; i++) {
		double r[3], v[3];
		getRVForDate(&satellites[satellite_indices[i]].tle, e_phys.epoch_ms, r, v);
		glm_vec3_copy((vec3){ r[0], r[1], r[2] }, satellite_verts[satellite_indices[i]]);
	}
}

void satellites_phys(void)
{
	if (!n_satellites)
		return;

	size_t i;
	for (i = 0; i < n_satellite_orbits; i++) {
		struct OrbitData *data = &g_array_index(satellite_orbit_data, struct OrbitData, i);
		gint64 diff = e_phys.epoch_ms - data->start_epoch_ms;
		if (ABS(diff) > data->max_delta_ms) {
			orbits_changed_phys = TRUE;
			data->start_epoch_ms = e_phys.epoch_ms;
			gen_satellite_orbit(data->idx);
		}
	}
}

void satellites_phys_sync(void)
{
	if (!n_satellites)
		return;

	if (orbits_changed_phys) {
		orbits_changed_phys = FALSE;
		bo_buffer(&vbo_orbits, satellite_orbits, satellite_orbits_size);
	}
}

void satellites_render(void)
{
	shader_bind(&shader);
	camera_mvp_generate(&e_camera, &e_phys.teme_to_world, transform);
	glUniformMatrix4fv(LOCU_TRANSFORM, 1, GL_FALSE, (const GLfloat *)&transform);

	if (satellites_renderable) {
		vao_bind(&vao_satellites);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glDrawElements(GL_POINTS, n_satellites_render, GL_UNSIGNED_INT, satellite_indices);
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

		vao_bind(&vao_orbits);
		glMultiDrawArrays(GL_LINE_LOOP, (GLint *)satellite_orbit_firsts->data, (GLint *)satellite_orbit_counts->data, n_satellite_orbits);
	}
}

void gen_satellite_orbit(guint32 idx)
{
	gint64 period = MS_IN_DAY / (float)satellites[idx].tle.n;
	int count = (int)g_array_index(satellite_orbit_counts, GLsizei, satellites[idx].orbit_idx);
	GLint first = g_array_index(satellite_orbit_firsts, GLint, satellites[idx].orbit_idx);
	int i;
	for (i = 0; i < count; i++) {
		gint64 epoch_ms = e_phys.epoch_ms + period * i / count;
		double r[3], v[3];
		getRVForDate(&satellites[idx].tle, epoch_ms, r, v);
		glm_vec3_copy((vec3){ r[0], r[1], r[2] }, satellite_orbits[first + i]);
		set_satellite_color(satellites[idx].satcat.opstat, satellite_orbit_colors[first + i]);
	}
}

void satellite_toggle_orbit(guint32 idx)
{
	if (satellites[idx].orbit_idx == UINT32_MAX) { /* Enable orbit */
		/* For similar-length line segments, apply Kepler's Third Law (T^2 prop. r^3) */
		int count = ORBIT_SEGMENT_LENGTH / cbrtf(satellites[idx].tle.n * satellites[idx].tle.n);

		satellites[idx].orbit_idx = n_satellite_orbits;

		struct OrbitData new_orbit_data = (struct OrbitData){
			.idx = idx,
			.start_epoch_ms = e_phys.epoch_ms,
			.max_delta_ms = satellites[idx].satcat.period * (60000L / ORBIT_MAX_DELTA_DIV),
		};
		g_array_append_val(satellite_orbit_data, new_orbit_data);
		g_array_append_val(satellite_orbit_counts, count);
		GLint first = n_satellite_orbits ? (g_array_index(satellite_orbit_firsts, GLint, n_satellite_orbits - 1) + g_array_index(satellite_orbit_counts, GLsizei, n_satellite_orbits - 1)) : 0;
		g_array_append_val(satellite_orbit_firsts, first);

		satellite_orbits_size += count * sizeof(vec3);
		satellite_orbits = g_realloc(satellite_orbits, satellite_orbits_size);
		satellite_orbit_colors = g_realloc(satellite_orbit_colors, satellite_orbits_size);

		gen_satellite_orbit(idx);

		bo_buffer(&vbo_orbits, satellite_orbits, satellite_orbits_size);
		bo_buffer(&vbo_orbit_colors, satellite_orbit_colors, satellite_orbits_size);

		n_satellite_orbits++;
	} else { /* Disable orbit */
		guint32 orbit_idx = satellites[idx].orbit_idx;
		satellites[idx].orbit_idx = UINT32_MAX;

		size_t count = g_array_index(satellite_orbit_counts, GLsizei, orbit_idx);

		size_t count_move = 0;
		guint32 i;
		for (i = orbit_idx + 1; i < n_satellite_orbits; i++) {
			g_array_index(satellite_orbit_firsts, GLsizei, i) -= count;
			satellites[g_array_index(satellite_orbit_data, struct OrbitData, i).idx].orbit_idx--;
			count_move += g_array_index(satellite_orbit_counts, GLsizei, i);
		}

		size_t size_move = count_move * sizeof(vec3);
		size_t size_remove = g_array_index(satellite_orbit_counts, GLsizei, orbit_idx) * sizeof(vec3);
		memmove(((guint8 *)satellite_orbits) + (satellite_orbits_size - size_move - size_remove), ((guint8 *)satellite_orbits) + (satellite_orbits_size - size_move), size_move);
		memmove(((guint8 *)satellite_orbit_colors) + (satellite_orbits_size - size_move - size_remove), ((guint8 *)satellite_orbit_colors) + (satellite_orbits_size - size_move), size_move);
		satellite_orbits_size -= size_remove;
		bo_buffer(&vbo_orbits, satellite_orbits, satellite_orbits_size);
		bo_buffer(&vbo_orbit_colors, satellite_orbit_colors, satellite_orbits_size);

		g_array_remove_index(satellite_orbit_firsts, orbit_idx);
		g_array_remove_index(satellite_orbit_counts, orbit_idx);
		g_array_remove_index(satellite_orbit_data, orbit_idx);

		n_satellite_orbits--;
	}
}

void satellite_select(double xpos, double ypos)
{
	vec3 p1, p2;
	glm_unproject((vec3){ xpos, e_gl_ctx.res_y - ypos, 0 }, transform, (vec4){ 0, 0, e_gl_ctx.res_x, e_gl_ctx.res_y }, p1);
	glm_unproject((vec3){ xpos, e_gl_ctx.res_y - ypos, 1 }, transform, (vec4){ 0, 0, e_gl_ctx.res_x, e_gl_ctx.res_y }, p2);

	vec3 dir;
	glm_vec3_sub(p2, p1, dir);
	float dir_inv_len2 = 1.f / glm_vec3_norm2(dir);

	float min_dist2 = FLT_MAX;
	guint32 idx;

	size_t i;
	for (i = 0; i < n_satellites; i++) {
		vec3 p1sat;
		glm_vec3_sub(satellite_verts[i], p1, p1sat);
		float dp = glm_vec3_dot(dir, p1sat);
		float dist2 = glm_vec3_norm2(p1sat);
		float cos_ang2 = (dp * dp * dir_inv_len2) / dist2;
		if (unlikely(cos_ang2 > SATELLITE_SELECT_MIN_COS_ANG2 && dp > 0 && dist2 < min_dist2)) {
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
