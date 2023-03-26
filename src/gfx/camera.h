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

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <cglm/mat4.h>
#include <cglm/vec3.h>

#undef near
#undef far

#define CAMERA_MIN_RAD 1.1f
#define CAMERA_MAX_RAD 10.f

struct Camera {
	vec3 pos;
	vec3 target;
	vec3 up;
	float rad;

	float fov;
	float near;
	float far;

	mat4 view_mat;
	mat4 proj_mat;
};

void camera_view_update(struct Camera *camera);
void camera_proj_update(struct Camera *camera);

void camera_mvp_generate(struct Camera *camera, mat4 *model, mat4 mvp);

void camera_zoom(struct Camera *camera, float offset);

extern struct Camera e_camera;

#endif /* __CAMERA_H__ */
