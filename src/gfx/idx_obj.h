#ifndef __IDX_OBJ_H__
#define __IDX_OBJ_H__

#include "type.h"

#include <cglm/vec2.h>
#include <cglm/vec3.h>

#include "bo.h"

struct IdxObj {
	size_t n_verts;
	size_t n_faces;

	vec3 *verts;
	vec2 *uv;
	guint32 *faces;

	struct BO vbo_verts;
	struct BO vbo_faces;
	struct BO vbo_uv;
};

/* Requires verts, uv, faces to already be set */
void idx_obj_init(struct IdxObj *idx_obj);

void idx_obj_deinit(struct IdxObj *idx_obj);

#endif /* __IDX_OBJ_H__ */
