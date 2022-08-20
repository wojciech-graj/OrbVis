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

#ifndef __SHADER_H__
#define __SHADER_H__

#include "gfx.h"

struct ShaderAttr {
	GLuint index;
	const GLchar *name;
};

struct Shader {
	GLuint handle;
};

void shader_init(struct Shader *shader, char *vs_path, char *fs_path, size_t n_vertex_attr, struct ShaderAttr vertex_attr[]);
void shader_deinit(struct Shader *shader);
void shader_bind(struct Shader *shader);

#endif /* __SHADER_H__ */
