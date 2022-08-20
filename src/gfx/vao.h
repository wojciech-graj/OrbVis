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

#ifndef __VAO_H__
#define __VAO_H__

#include "bo.h"
#include "gfx.h"

struct VAO {
	GLuint handle;
};

void vao_init(struct VAO *vao);
void vao_deinit(struct VAO *vao);
void vao_bind(struct VAO *vao);
void vao_attr(struct VAO *vao, struct BO *vbo, GLuint index, GLint size, GLenum type, GLsizei stride, size_t offset);

#endif /* __VAO_H__ */
