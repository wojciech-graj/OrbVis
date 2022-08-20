/*
 * Copyright (c) 2022 Wojciech Graj
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
