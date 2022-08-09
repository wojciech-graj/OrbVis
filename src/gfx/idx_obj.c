#include "idx_obj.h"

void idx_obj_init(struct IdxObj *idx_obj)
{
	bo_init(&idx_obj->vbo_verts, GL_ARRAY_BUFFER, FALSE);
	bo_init(&idx_obj->vbo_faces, GL_ELEMENT_ARRAY_BUFFER, FALSE);
	bo_init(&idx_obj->vbo_uv, GL_ARRAY_BUFFER, FALSE);

	bo_buffer(&idx_obj->vbo_verts, idx_obj->verts, sizeof(vec3) * idx_obj->n_verts);
	bo_buffer(&idx_obj->vbo_faces, idx_obj->faces, sizeof(guint32[3]) * idx_obj->n_faces);
	bo_buffer(&idx_obj->vbo_uv, idx_obj->uv, sizeof(vec2) * idx_obj->n_verts);
}

void idx_obj_deinit(struct IdxObj *idx_obj)
{
	bo_deinit(&idx_obj->vbo_verts);
	bo_deinit(&idx_obj->vbo_faces);
	bo_deinit(&idx_obj->vbo_uv);

	g_free(idx_obj->verts);
	g_free(idx_obj->faces);
	g_free(idx_obj->uv);
}
