#include "idx_obj.h"

void idx_obj_init(struct IdxObj *idx_obj)
{
	bo_init(&idx_obj->vbo_verts, GL_ARRAY_BUFFER, false);
	bo_init(&idx_obj->vbo_faces, GL_ELEMENT_ARRAY_BUFFER, false);
	bo_init(&idx_obj->vbo_uv, GL_ARRAY_BUFFER, false);

	bo_buffer(&idx_obj->vbo_verts, idx_obj->verts, sizeof(vec3) * idx_obj->n_verts);
	bo_buffer(&idx_obj->vbo_faces, idx_obj->faces, sizeof(uint32_t[3]) * idx_obj->n_faces);
	bo_buffer(&idx_obj->vbo_uv, idx_obj->uv, sizeof(vec2) * idx_obj->n_verts);
}

void idx_obj_deinit(struct IdxObj *idx_obj)
{
	bo_deinit(&idx_obj->vbo_verts);
	bo_deinit(&idx_obj->vbo_faces);
	bo_deinit(&idx_obj->vbo_uv);

	free(idx_obj->verts);
	free(idx_obj->faces);
	free(idx_obj->uv);
}
