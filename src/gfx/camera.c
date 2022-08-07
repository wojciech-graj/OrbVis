#include "camera.h"

#include <cglm/cam.h>

#include "gfx.h"

struct Camera g_camera;

void camera_view_update(struct Camera *camera)
{
	glm_lookat(camera->pos, camera->target, camera->up, camera->view_mat);
}

void camera_proj_update(struct Camera *camera)
{
	glm_perspective(camera->fov, g_gl_ctx.res_x / (float)g_gl_ctx.res_y, camera->near, camera->far, camera->proj_mat);
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
