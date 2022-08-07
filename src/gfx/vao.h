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
