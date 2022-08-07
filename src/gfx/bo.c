#include "bo.h"

void bo_init(struct BO *bo, GLint type, bool dynamic)
{
	bo->type = type;
	bo->dynamic = dynamic;
	glGenBuffers(1, &bo->handle);
}

void bo_deinit(struct BO *bo)
{
	glDeleteBuffers(1, &bo->handle);
}

void bo_bind(struct BO *bo)
{
	glBindBuffer(bo->type, bo->handle);
}

void bo_buffer(struct BO *bo, void *data, size_t size)
{
	bo_bind(bo);
	glBufferData(bo->type, size, data, bo->dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}
