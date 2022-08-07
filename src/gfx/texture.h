#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "gfx.h"

struct Texture {
	GLuint handle;
	GLenum unit;
	GLenum type;
};

void texture_init(struct Texture *texture, GLenum unit, GLenum type);

void texture_init_from_image(struct Texture *texture, const char *filename, GLenum unit, GLenum type);

void texture_deinit(struct Texture *texture);

void texture_bind(struct Texture *texture);

#endif /* __TEXTURE_H__ */
