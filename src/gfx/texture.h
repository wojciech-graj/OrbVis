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

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "gfx.h"

struct Texture {
	GLuint handle;
	GLenum unit;
	GLenum type;
};

void textures_init(void);

void texture_init(struct Texture *texture, GLenum unit, GLenum type);

void texture_init_from_image(struct Texture *texture, unsigned char *buffer, unsigned int len, GLenum unit, GLenum type);

void texture_deinit(struct Texture *texture);

void texture_bind(struct Texture *texture);

#endif /* __TEXTURE_H__ */
