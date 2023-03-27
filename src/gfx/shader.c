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

#include "shader.h"

#include "error.h"
#include "error_gfx.h"
#include "system.h"

static GLint shader_compile(const char *str, unsigned int len, GLenum type);

GLint shader_compile(const char *str, unsigned int len, GLenum type)
{
	GLuint handle = glCreateShader(type);
	glShaderSource(handle, 1, (const GLchar *const *)&str, (const GLint *)&len);
	glCompileShader(handle);

	GLint compiled;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);
	gfx_error_check(compiled, glGetShaderInfoLog, handle, "Failed to compile shader.");

	return handle;
}

void shader_init(struct Shader *shader, const char *vs, unsigned int vs_len, const char *fs, unsigned int fs_len, size_t n_vertex_attr, struct ShaderAttr vertex_attr[])
{
	GLuint vs_handle = shader_compile(vs, vs_len, GL_VERTEX_SHADER);
	GLuint fs_handle = shader_compile(fs, fs_len, GL_FRAGMENT_SHADER);
	shader->handle = glCreateProgram();

	glAttachShader(shader->handle, vs_handle);
	glAttachShader(shader->handle, fs_handle);

	size_t i;
	for (i = 0; i < n_vertex_attr; i++)
		glBindAttribLocation(shader->handle, vertex_attr[i].index, vertex_attr[i].name);

	glLinkProgram(shader->handle);

	GLint linked;
	glGetProgramiv(shader->handle, GL_LINK_STATUS, &linked);
	gfx_error_check(linked, glGetProgramInfoLog, shader->handle, "Failed to link shaders.");

	glDetachShader(shader->handle, vs_handle);
	glDetachShader(shader->handle, fs_handle);

	glDeleteShader(vs_handle);
	glDeleteShader(fs_handle);
}

void shader_deinit(struct Shader *shader)
{
	glDeleteProgram(shader->handle);
}

void shader_bind(struct Shader *shader)
{
	glUseProgram(shader->handle);
}
