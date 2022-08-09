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
