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

#include "camera.h"

#include <cglm/cam.h>

#include "gfx.h"

struct Camera e_camera;

void camera_view_update(struct Camera *camera)
{
	glm_lookat(camera->pos, camera->target, camera->up, camera->view_mat);
}

void camera_proj_update(struct Camera *camera)
{
	glm_perspective(camera->fov, e_gl_ctx.res_x / (float)e_gl_ctx.res_y, camera->near, camera->far, camera->proj_mat);
}

void camera_mvp_generate(struct Camera *camera, mat4 *model, mat4 mvp)
{
	glm_mat4_mulN((mat4 *[]){ &camera->proj_mat, &camera->view_mat, model }, 3, mvp);
}

void camera_zoom(struct Camera *camera, float offset)
{
	float rad = glm_clamp(camera->rad - offset, 1.1f, 10.f);
	float d_rad = rad - camera->rad;
	float d_rad_pct = (camera->rad + d_rad) / (camera->rad);
	camera->rad = rad;
	glm_vec3_scale(camera->pos, d_rad_pct, camera->pos);
	camera_view_update(camera);
}
