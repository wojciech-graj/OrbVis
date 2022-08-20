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

#include "idx_obj.h"

void idx_obj_init(struct IdxObj *idx_obj)
{
	bo_init(&idx_obj->vbo_verts, GL_ARRAY_BUFFER, FALSE);
	bo_init(&idx_obj->vbo_faces, GL_ELEMENT_ARRAY_BUFFER, FALSE);
	bo_init(&idx_obj->vbo_uv, GL_ARRAY_BUFFER, FALSE);

	bo_buffer(&idx_obj->vbo_verts, idx_obj->verts, sizeof(vec3) * idx_obj->n_verts);
	bo_buffer(&idx_obj->vbo_faces, idx_obj->faces, sizeof(guint32[3]) * idx_obj->n_faces);
	bo_buffer(&idx_obj->vbo_uv, idx_obj->uv, sizeof(vec2) * idx_obj->n_verts);
}

void idx_obj_deinit(struct IdxObj *idx_obj)
{
	bo_deinit(&idx_obj->vbo_verts);
	bo_deinit(&idx_obj->vbo_faces);
	bo_deinit(&idx_obj->vbo_uv);

	g_free(idx_obj->verts);
	g_free(idx_obj->faces);
	g_free(idx_obj->uv);
}
