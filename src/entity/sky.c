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

#include "sky.h"

#include <cglm/affine.h>
#include <cglm/mat4.h>
#include <cglm/vec3.h>

#include "camera.h"
#include "gfx.h"
#include "icosphere.h"
#include "idx_obj.h"
#include "model.h"
#include "shader.h"
#include "texture.h"
#include "vao.h"

enum LayoutLoc {
	LOCL_APOS = 0u,
	LOCL_ATEXCOORD,
};

enum UniformLoc {
	LOCU_TEXTURE = 0u,
	LOCU_TRANSFORM,
};

extern const unsigned char res_shader_sky_vert[];
extern const unsigned int res_shader_sky_vert_len;
extern const unsigned char res_shader_sky_frag[];
extern const unsigned int res_shader_sky_frag_len;

extern const unsigned char res_texture_stars_jpg[];
extern const unsigned int res_texture_stars_jpg_len;

static struct IdxObj obj;
static struct VAO vao;
static struct Model model;
static struct Shader shader;
static struct Texture texture;

void sky_init(void)
{
	icosphere_generate(1, &obj.verts, &obj.faces, &obj.uv, &obj.n_verts, &obj.n_faces);

	vao_init(&vao);
	idx_obj_init(&obj);
	model_init(&model, &obj);
	glm_scale_make(model.scl, (vec3){ 50.f, 50.f, 50.f });
	model_transform(&model);

	shader_init(&shader, res_shader_sky_vert, res_shader_sky_vert_len, res_shader_sky_frag, res_shader_sky_frag_len, 2, (struct ShaderAttr[]){
																    { LOCL_APOS, "in_pos" },
																    { LOCL_ATEXCOORD, "in_uv" },
															    });

	texture_init_from_image(&texture, res_texture_stars_jpg, res_texture_stars_jpg_len, GL_TEXTURE0, GL_TEXTURE_2D);

	vao_attr(&vao, &obj.vbo_verts, 0, 3, GL_FLOAT, sizeof(vec3), 0);
	vao_attr(&vao, &obj.vbo_uv, 1, 2, GL_FLOAT, sizeof(vec2), 0);
}

void sky_deinit(void)
{
	vao_deinit(&vao);
	idx_obj_deinit(&obj);
	shader_deinit(&shader);
	texture_deinit(&texture);
}

void sky_render(void)
{
	vao_bind(&vao);
	bo_bind(&obj.vbo_faces);
	texture_bind(&texture);

	mat4 transform;
	camera_mvp_generate(&e_camera, &model.model_mat, transform);

	shader_bind(&shader);
	glUniform1i(LOCU_TEXTURE, 0);
	glUniformMatrix4fv(LOCU_TRANSFORM, 1, GL_FALSE, (const GLfloat *)&transform);

	glDrawElements(GL_TRIANGLES, obj.n_faces * 3, GL_UNSIGNED_INT, (GLvoid *)0);
}
