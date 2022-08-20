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

#include "vao.h"

void vao_init(struct VAO *vao)
{
	glGenVertexArrays(1, &vao->handle);
}

void vao_deinit(struct VAO *vao)
{
	glDeleteVertexArrays(1, &vao->handle);
}

void vao_bind(struct VAO *vao)
{
	glBindVertexArray(vao->handle);
}

void vao_attr(struct VAO *vao, struct BO *vbo, GLuint index, GLint size, GLenum type, GLsizei stride, size_t offset)
{
	vao_bind(vao);
	bo_bind(vbo);

	switch (type) {
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_INT_2_10_10_10_REV:
	case GL_UNSIGNED_INT_2_10_10_10_REV:
		glVertexAttribIPointer(index, size, type, stride, (void *)offset);
		break;
	default:
		glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void *)offset);
		break;
	}
	glEnableVertexAttribArray(index);
}
