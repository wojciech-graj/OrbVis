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

#include "render.h"

#include "bo.h"
#include "camera.h"
#include "entity.h"
#include "error.h"
#include "gfx.h"
#include "icosphere.h"
#include "idx_obj.h"
#include "model.h"
#include "shader.h"
#include "system.h"
#include "texture.h"
#include "ui.h"
#include "vao.h"

#include <cglm/cglm.h>

#define DEFAULT_CAMERA_POS ((vec3){ 0.f, 5.f, 0.f })
#define DEFAULT_CAMERA_RAD 5.f
#define CAMERA_FOV ((float)G_PI / 2.f)
#define CAMERA_NEAR 0.05f
#define CAMERA_FAR 100.f

void render_init(void)
{
	glm_vec3_copy(DEFAULT_CAMERA_POS, e_camera.pos);
	glm_vec3_copy((vec3){ 0, 0, 0 }, e_camera.target);
	glm_vec3_copy((vec3){ 0, 0, 1 }, e_camera.up);

	e_camera.rad = DEFAULT_CAMERA_RAD;
	e_camera.fov = CAMERA_FOV;
	e_camera.near = CAMERA_NEAR;
	e_camera.far = CAMERA_FAR;

	camera_view_update(&e_camera);
	camera_proj_update(&e_camera);

	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_STENCIL_TEST);
}

void render_process(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	entity_render();
}
