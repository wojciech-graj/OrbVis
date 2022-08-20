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

#include "bo.h"

void bo_init(struct BO *bo, GLint type, gboolean dynamic)
{
	bo->type = type;
	bo->dynamic = dynamic;
	glGenBuffers(1, &bo->handle);
}

void bo_deinit(struct BO *bo)
{
	glDeleteBuffers(1, &bo->handle);
}

void bo_bind(struct BO *bo)
{
	glBindBuffer(bo->type, bo->handle);
}

void bo_buffer(struct BO *bo, void *data, size_t size)
{
	bo_bind(bo);
	glBufferData(bo->type, size, data, bo->dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}
