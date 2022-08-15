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

void render_init(void)
{
	glm_vec3_copy((vec3){ 0, 5.0, 0 }, e_camera.pos);
	glm_vec3_copy((vec3){ 0, 0, 0 }, e_camera.target);
	glm_vec3_copy((vec3){ 0, 0, 1 }, e_camera.up);

	e_camera.rad = 5.f;
	e_camera.fov = (float)G_PI / 2.f;
	e_camera.near = 0.05f;
	e_camera.far = 100.f;

	camera_view_update(&e_camera);
	camera_proj_update(&e_camera);

	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_STENCIL_TEST);
}

void render_deinit(void)
{
}

void render_process(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	entity_render();
	//ui_render();
}
