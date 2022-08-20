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

#ifndef __BO_H__
#define __BO_H__

#include "gfx.h"

struct BO {
	GLuint handle;
	GLint type;
	gboolean dynamic;
};

void bo_init(struct BO *bo, GLint type, gboolean dynamic);
void bo_deinit(struct BO *bo);

void bo_bind(struct BO *bo);
void bo_buffer(struct BO *bo, void *data, size_t size);

#endif /* __BO_H__ */
