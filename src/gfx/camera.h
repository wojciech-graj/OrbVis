#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <cglm/mat4.h>
#include <cglm/vec3.h>

#undef near
#undef far

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

extern struct Camera g_camera;

#endif /* __CAMERA_H__ */
