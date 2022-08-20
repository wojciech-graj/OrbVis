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

#include "texture.h"

#include "stb_image.h"

#include "error.h"

void texture_init(struct Texture *texture, GLenum unit, GLenum type)
{
	texture->unit = unit;
	texture->type = type;
	glActiveTexture(unit);
	glGenTextures(1, &texture->handle);
	glBindTexture(type, texture->handle);
}

void texture_init_from_image(struct Texture *texture, const char *filename, GLenum unit, GLenum type)
{
	int width, height, n_channels;
	stbi_set_flip_vertically_on_load(TRUE); /* TODO: Move to init */
	unsigned char *data = stbi_load(filename, &width, &height, &n_channels, 0);
	error_check(data, "Failed to open texture [%s].", filename);

	texture_init(texture, unit, type);

	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (type == GL_TEXTURE_2D)
		glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLenum format = (n_channels == 3) ? GL_RGB : GL_RED;

	if (type == GL_TEXTURE_1D)
		glTexImage1D(GL_TEXTURE_1D, 0, format, width, 0, format, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(type);

	stbi_image_free(data);
}

void texture_deinit(struct Texture *texture)
{
	glDeleteTextures(1, &texture->handle);
}

void texture_bind(struct Texture *texture)
{
	glActiveTexture(texture->unit);
	glBindTexture(texture->type, texture->handle);
}
