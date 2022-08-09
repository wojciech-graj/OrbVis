#include "icosphere.h"

#include "hashmap.h"

#include "system.h"

struct Edge {
	union {
		struct {
			guint32 a : 32;
			guint32 b : 32;
		} v;
		gint64 hash;
	} edge;
	guint32 mid;
};

struct Vertex {
	guint32 orig;
	guint32 new;
};

#define N 0.f
#define X .525731112119133606f
#define Z .850650808352039932f

static vec3 icosphere_verts[12] = {
	{ -X, N, Z },
	{ X, N, Z },
	{ -X, N, -Z },
	{ X, N, -Z },
	{ N, Z, X },
	{ N, Z, -X },
	{ N, -Z, X },
	{ N, -Z, -X },
	{ Z, X, N },
	{ -Z, X, N },
	{ Z, -X, N },
	{ -Z, -X, N },
};

static const guint32 icosphere_faces[20][3] = {
	{ 0, 4, 1 },
	{ 0, 9, 4 },
	{ 9, 5, 4 },
	{ 4, 5, 8 },
	{ 4, 8, 1 },
	{ 8, 10, 1 },
	{ 8, 3, 10 },
	{ 5, 3, 8 },
	{ 5, 2, 3 },
	{ 2, 7, 3 },
	{ 7, 10, 3 },
	{ 7, 6, 10 },
	{ 7, 11, 6 },
	{ 11, 0, 6 },
	{ 0, 1, 6 },
	{ 6, 1, 10 },
	{ 9, 0, 11 },
	{ 9, 11, 2 },
	{ 9, 2, 5 },
	{ 7, 2, 11 },
};

static guint64 edge_hash(const void *item, guint64 seed0, guint64 seed1);
static int edge_compare(const void *a, const void *b, void *udata);
static guint64 vertex_hash(const void *item, guint64 seed0, guint64 seed1);
static int vertex_compare(const void *a, const void *b, void *udata);
static void subdivide(guint32 *face, guint32 *next_faces, vec3 *verts, size_t *vert_cnt, struct hashmap *map);
static void fix_uv_wrapping(vec3 *verts, guint32 *faces, vec2 *uv, size_t *vert_cnt, size_t n_faces);
static void separate_pole_vertex(unsigned idx, guint32 *trig, gboolean *fnd, vec3 *verts, vec2 *uv, size_t *vert_cnt);
static void separate_pole_vertices(vec3 *verts, guint32 *faces, vec2 *uv, size_t *vert_cnt, size_t n_faces);

static guint64 edge_hash(const void *item, guint64 seed0, guint64 seed1)
{
	(void)seed0;
	(void)seed1;
	return ((struct Edge *)item)->edge.hash;
}

static int edge_compare(const void *a, const void *b, void *udata)
{
	(void)udata;
	return ((struct Edge *)a)->edge.hash - ((struct Edge *)b)->edge.hash;
}

static guint64 vertex_hash(const void *item, guint64 seed0, guint64 seed1)
{
	(void)seed0;
	(void)seed1;
	return ((struct Vertex *)item)->orig;
}

static int vertex_compare(const void *a, const void *b, void *udata)
{
	(void)udata;
	return (((struct Vertex *)a)->orig) - (((struct Vertex *)b)->orig);
}

static void subdivide(guint32 *face, guint32 *next_faces, vec3 *verts, size_t *vert_cnt, struct hashmap *map)
{
	/* Generate vertices */
	static const unsigned face_idxs[3][2] = {
		{ 0, 1 },
		{ 0, 2 },
		{ 1, 2 },
	};

	guint32 mid_verts[3];
	unsigned i;
#pragma GCC unroll 3
	for (i = 0; i < 3; i++) {
		guint32 f0 = face[face_idxs[i][0]];
		guint32 f1 = face[face_idxs[i][1]];
		struct Edge *em;
		if (f0 > f1)
			SWAP(f0, f1);
		em = hashmap_get(map, &(struct Edge){
					      .edge.v = { f0, f1 },
				      });

		if (em) {
			mid_verts[i] = em->mid;
		} else {
			mid_verts[i] = *vert_cnt;
			glm_vec3_add(verts[f0], verts[f1], verts[*vert_cnt]);
			glm_vec3_scale(verts[*vert_cnt], 0.5f, verts[*vert_cnt]);
			glm_vec3_normalize(verts[*vert_cnt]);
			hashmap_set(map, &(struct Edge){
						 .edge.v = { f0, f1 },
						 .mid = *vert_cnt,
					 });
			(*vert_cnt)++;
		}
	}

	/* Generate faces */
	next_faces[0] = face[0];
	next_faces[1] = mid_verts[0];
	next_faces[2] = mid_verts[1];
	next_faces[3] = mid_verts[0];
	next_faces[4] = face[1];
	next_faces[5] = mid_verts[2];
	next_faces[6] = mid_verts[1];
	next_faces[7] = mid_verts[2];
	next_faces[8] = face[2];
	face[0] = mid_verts[1];
	face[1] = mid_verts[0];
	face[2] = mid_verts[2];
}

/* Algorithm based on https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/ */
void icosphere_generate(const unsigned n_sub, vec3 **verts, guint32 **faces, vec2 **uv, size_t *n_verts, size_t *n_faces)
{
	size_t pow_4_n_sub = 1u << (2 * n_sub); /*4^n_sub*/
	*n_faces = 20 * pow_4_n_sub;
	*n_verts = (10 * pow_4_n_sub + 2)
		+ (n_sub ? (3 * (1u << n_sub) + 9) : 1); /* duplicated verts for UV wrapping + separate pole vertices */

	*verts = g_malloc(sizeof(vec3) * *n_verts);
	*faces = g_malloc(sizeof(guint32[3]) * *n_faces);
	*uv = g_malloc(sizeof(vec2) * *n_faces);

	memcpy(*faces, icosphere_faces, sizeof(icosphere_faces));
	memcpy(*verts, icosphere_verts, sizeof(icosphere_verts));

	struct hashmap *map = hashmap_new(sizeof(struct Edge), 4 * *n_verts / 3 + 1, 0, 0, edge_hash, edge_compare, NULL, NULL);

	/* Recursively subdivide */
	size_t n_trigs = 20;
	size_t vert_cnt = 12;
	size_t i;
	for (i = 0; i < n_sub; i++) {
		size_t j;
		for (j = 0; j < n_trigs; j++)
			subdivide(*faces + j * 3, *faces + 3 * n_trigs + j * 9, *verts, &vert_cnt, map);
		n_trigs *= 4;
	}

	hashmap_free(map);

	/* Generate UVs */
	for (i = 0; i < *n_verts; i++) {
		glm_vec2_copy((vec2){
				      (atan2f((*verts)[i][1], (*verts)[i][0]) + PI) * (1.f / (2.f * PI)),
				      acosf(-((*verts)[i][2])) * (1.f / PI) },
			(*uv)[i]);
	}

	/* Fix wrapped UV (de-zippering) */
	fix_uv_wrapping(*verts, *faces, *uv, &vert_cnt, *n_faces);

	/* Fix pole vertices */
	if (n_sub)
		separate_pole_vertices(*verts, *faces, *uv, &vert_cnt, *n_faces);
}

static void fix_uv_wrapping(vec3 *verts, guint32 *faces, vec2 *uv, size_t *vert_cnt, size_t n_faces)
{
	struct hashmap *map = hashmap_new(sizeof(struct Vertex), 10, 0, 0, vertex_hash, vertex_compare, NULL, NULL);

	size_t i;
	for (i = 0; i < n_faces; i++) {
		guint32 *trig = faces + i * 3;
		float norm = ((uv[trig[1]][0] - uv[trig[0]][0]) * (uv[trig[2]][1] - uv[trig[0]][1]))
			- ((uv[trig[1]][1] - uv[trig[0]][1]) * (uv[trig[2]][0] - uv[trig[0]][0]));
		if (norm > 0) {
			unsigned j;
#pragma GCC unroll 3
			for (j = 0; j < 3; j++) {
				if (uv[trig[j]][0] < 0.25f) {
					struct Vertex *vert = hashmap_get(map, &(struct Vertex){ .orig = trig[j] });
					if (!vert) {
						glm_vec3_copy(verts[trig[j]], verts[*vert_cnt]);
						glm_vec2_copy((vec2){
								      uv[trig[j]][0] + 1.f,
								      uv[trig[j]][1],
							      },
							uv[*vert_cnt]);
						hashmap_set(map, &(struct Vertex){ .orig = trig[j], .new = *vert_cnt });
						trig[j] = *vert_cnt;
						(*vert_cnt)++;
					} else {
						trig[j] = vert->new;
					}
				}
			}
		}
	}

	hashmap_free(map);
}

static void separate_pole_vertex(unsigned idx, guint32 *trig, gboolean *fnd, vec3 *verts, vec2 *uv, size_t *vert_cnt)
{
	float u_avg;
	switch (idx) {
	case 0:
		u_avg = (uv[trig[1]][0] + uv[trig[2]][0]) * 0.5f;
		break;
	case 1:
		u_avg = (uv[trig[0]][0] + uv[trig[2]][0]) * 0.5f;
		break;
	case 2:
		u_avg = (uv[trig[0]][0] + uv[trig[1]][0]) * 0.5f;
		break;
	}

	if (*fnd) {
		glm_vec2_copy((vec2){
				      u_avg,
				      uv[trig[idx]][1],
			      },
			uv[*vert_cnt]);
		glm_vec3_copy(verts[trig[idx]], verts[*vert_cnt]);
		trig[idx] = *vert_cnt;
		(*vert_cnt)++;
	} else {
		*fnd = TRUE;
		uv[trig[idx]][0] = u_avg;
	}
}

static void separate_pole_vertices(vec3 *verts, guint32 *faces, vec2 *uv, size_t *vert_cnt, size_t n_faces)
{
	size_t i;
	guint32 north, south;
	for (i = 0; i < *vert_cnt; i++)
		if (verts[i][2] > 1.f - 1e-6f) {
			north = i;
			break;
		}
	for (i = 0; i < *vert_cnt; i++)
		if (verts[i][2] < -1.f + 1e-6f) {
			south = i;
			break;
		}

	gboolean fnd_north = FALSE, fnd_south = FALSE;
	for (i = 0; i < n_faces; i++) {
		guint32 *trig = faces + i * 3;
		unsigned j;
#pragma GCC unroll 3
		for (j = 0; j < 3; j++) {
			if (unlikely(trig[j] == north)) {
				separate_pole_vertex(j, trig, &fnd_north, verts, uv, vert_cnt);
				break;
			} else if (unlikely(trig[j] == south)) {
				separate_pole_vertex(j, trig, &fnd_south, verts, uv, vert_cnt);
				break;
			}
		}
	}
}
