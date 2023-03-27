/*
 * Copyright (c) 2022-2023 Wojciech Graj
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

#include "earth.h"

#include <cglm/mat4.h>
#include <cglm/vec3.h>

#include "camera.h"
#include "gfx.h"
#include "icosphere.h"
#include "idx_obj.h"
#include "model.h"
#include "phys.h"
#include "setting.h"
#include "shader.h"
#include "texture.h"
#include "vao.h"

enum LayoutLoc {
	LOCL_APOS = 0u,
	LOCL_ATEXCOORD,
};

enum UniformLoc {
	LOCU_TEXTURE_DAY = 0,
	LOCU_TEXTURE_NIGHT,
	LOCU_TEXTURE_SPECULAR,
	LOCU_TEXTURE_CLOUDS,
	LOCU_TEXTURE_GRADIENT,
	LOCU_TRANSFORM,
	LOCU_SUN_DIR,
	LOCU_LOOK_POS,
	LOCU_CLOUDS,
	LOCU_LIGHTING,
};

extern const unsigned char res_shader_earth_vert[];
extern const unsigned int res_shader_earth_vert_len;
extern const unsigned char res_shader_earth_frag[];
extern const unsigned int res_shader_earth_frag_len;

extern const unsigned char res_texture_earth_daymap_jpg[];
extern const unsigned int res_texture_earth_daymap_jpg_len;
extern const unsigned char res_texture_earth_nightmap_jpg[];
extern const unsigned int res_texture_earth_nightmap_jpg_len;
extern const unsigned char res_texture_earth_specular_map_jpg[];
extern const unsigned int res_texture_earth_specular_map_jpg_len;
extern const unsigned char res_texture_earth_clouds_jpg[];
extern const unsigned int res_texture_earth_clouds_jpg_len;
extern const unsigned char res_texture_sky_gradient_jpg[];
extern const unsigned int res_texture_sky_gradient_jpg_len;

struct IdxObj obj_earth;
struct VAO vao_earth;
static struct Model model;

static struct Shader shader;
static struct Texture texture_day;
static struct Texture texture_night;
static struct Texture texture_specular;
static struct Texture texture_clouds;
static struct Texture texture_gradient;

void earth_init(void)
{
	icosphere_generate(3, &obj_earth.verts, &obj_earth.faces, &obj_earth.uv, &obj_earth.n_verts, &obj_earth.n_faces);

	vao_init(&vao_earth);

	idx_obj_init(&obj_earth);
	model_init(&model, &obj_earth);

	shader_init(&shader, (const char *)res_shader_earth_vert, res_shader_earth_vert_len, (const char *)res_shader_earth_frag, res_shader_earth_frag_len, 2, (struct ShaderAttr[]){
																					{ LOCL_APOS, "in_pos" },
																					{ LOCL_ATEXCOORD, "in_uv" },
																				});

	texture_init_from_image(&texture_day, res_texture_earth_daymap_jpg, res_texture_earth_daymap_jpg_len, GL_TEXTURE0, GL_TEXTURE_2D);
	texture_init_from_image(&texture_night, res_texture_earth_nightmap_jpg, res_texture_earth_nightmap_jpg_len, GL_TEXTURE1, GL_TEXTURE_2D);
	texture_init_from_image(&texture_specular, res_texture_earth_specular_map_jpg, res_texture_earth_specular_map_jpg_len, GL_TEXTURE2, GL_TEXTURE_2D);
	texture_init_from_image(&texture_clouds, res_texture_earth_clouds_jpg, res_texture_earth_clouds_jpg_len, GL_TEXTURE3, GL_TEXTURE_2D);
	texture_init_from_image(&texture_gradient, res_texture_sky_gradient_jpg, res_texture_sky_gradient_jpg_len, GL_TEXTURE4, GL_TEXTURE_1D);

	vao_attr(&vao_earth, &obj_earth.vbo_verts, 0, 3, GL_FLOAT, sizeof(vec3), 0);
	vao_attr(&vao_earth, &obj_earth.vbo_uv, 1, 2, GL_FLOAT, sizeof(vec2), 0);
}

void earth_deinit(void)
{
	vao_deinit(&vao_earth);
	idx_obj_deinit(&obj_earth);
	shader_deinit(&shader);
	texture_deinit(&texture_day);
	texture_deinit(&texture_night);
	texture_deinit(&texture_specular);
	texture_deinit(&texture_clouds);
	texture_deinit(&texture_gradient);
}

void earth_render(void)
{
	vao_bind(&vao_earth);
	bo_bind(&obj_earth.vbo_faces);
	texture_bind(&texture_day);
	texture_bind(&texture_night);
	texture_bind(&texture_specular);
	texture_bind(&texture_clouds);
	texture_bind(&texture_gradient);

	mat4 transform;
	camera_mvp_generate(&e_camera, &model.model_mat, transform);

	shader_bind(&shader);
	glUniform3fv(LOCU_SUN_DIR, 1, e_phys.sun_dir);
	glUniform1i(LOCU_TEXTURE_DAY, 0);
	glUniform1i(LOCU_TEXTURE_NIGHT, 1);
	glUniform1i(LOCU_TEXTURE_SPECULAR, 2);
	glUniform1i(LOCU_TEXTURE_CLOUDS, 3);
	glUniform1i(LOCU_TEXTURE_GRADIENT, 4);
	glUniform3fv(LOCU_LOOK_POS, 1, e_camera.pos);
	glUniformMatrix4fv(LOCU_TRANSFORM, 1, GL_FALSE, (const GLfloat *)&transform);
	glUniform1i(LOCU_CLOUDS, gs_clouds);
	glUniform1i(LOCU_LIGHTING, gs_lighting);

	glDrawElements(GL_TRIANGLES, obj_earth.n_faces * 3, GL_UNSIGNED_INT, (GLvoid *)0);
}
