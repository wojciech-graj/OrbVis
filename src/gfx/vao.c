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
