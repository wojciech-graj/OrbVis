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

struct IdxObj g_obj_earth;
struct VAO g_vao_earth;
static struct Model model;

static struct Shader shader;
static struct Texture texture_day;
static struct Texture texture_night;
static struct Texture texture_specular;
static struct Texture texture_clouds;
static struct Texture texture_gradient;

void earth_init(void)
{
	icosphere_generate(3, &g_obj_earth.verts, &g_obj_earth.faces, &g_obj_earth.uv, &g_obj_earth.n_verts, &g_obj_earth.n_faces);

	vao_init(&g_vao_earth);

	idx_obj_init(&g_obj_earth);
	model_init(&model, &g_obj_earth);

	shader_init(&shader, "res/shader/earth.vert", "res/shader/earth.frag", 2, (struct ShaderAttr[]){
											  { LOCL_APOS, "in_pos" },
											  { LOCL_ATEXCOORD, "in_uv" },
										  });

	texture_init_from_image(&texture_day, "res/texture/8k_earth_daymap.jpg", GL_TEXTURE0, GL_TEXTURE_2D);
	texture_init_from_image(&texture_night, "res/texture/8k_earth_nightmap.jpg", GL_TEXTURE1, GL_TEXTURE_2D);
	texture_init_from_image(&texture_specular, "res/texture/8k_earth_specular_map.jpg", GL_TEXTURE2, GL_TEXTURE_2D);
	texture_init_from_image(&texture_clouds, "res/texture/8k_earth_clouds.jpg", GL_TEXTURE3, GL_TEXTURE_2D);
	texture_init_from_image(&texture_clouds, "res/texture/sky_gradient.jpg", GL_TEXTURE4, GL_TEXTURE_1D);

	vao_attr(&g_vao_earth, &g_obj_earth.vbo_verts, 0, 3, GL_FLOAT, sizeof(vec3), 0);
	vao_attr(&g_vao_earth, &g_obj_earth.vbo_uv, 1, 2, GL_FLOAT, sizeof(vec2), 0);
}

void earth_deinit(void)
{
	vao_deinit(&g_vao_earth);
	idx_obj_deinit(&g_obj_earth);
	shader_deinit(&shader);
	texture_deinit(&texture_day);
	texture_deinit(&texture_night);
	texture_deinit(&texture_specular);
	texture_deinit(&texture_clouds);
	texture_deinit(&texture_gradient);
}

void earth_render(void)
{
	vao_bind(&g_vao_earth);
	bo_bind(&g_obj_earth.vbo_faces);
	texture_bind(&texture_day);
	texture_bind(&texture_night);
	texture_bind(&texture_specular);
	texture_bind(&texture_clouds);
	texture_bind(&texture_gradient);

	mat4 transform;
	camera_mvp_generate(&g_camera, &model.model_mat, transform);

	shader_bind(&shader);
	glUniform3fv(LOCU_SUN_DIR, 1, g_phys.sun_dir);
	glUniform1i(LOCU_TEXTURE_DAY, 0);
	glUniform1i(LOCU_TEXTURE_NIGHT, 1);
	glUniform1i(LOCU_TEXTURE_SPECULAR, 2);
	glUniform1i(LOCU_TEXTURE_CLOUDS, 3);
	glUniform1i(LOCU_TEXTURE_GRADIENT, 4);
	glUniform3fv(LOCU_LOOK_POS, 1, g_camera.pos);
	glUniformMatrix4fv(LOCU_TRANSFORM, 1, GL_FALSE, (const GLfloat *)&transform);
	glUniform1i(LOCU_CLOUDS, gs_clouds);
	glUniform1i(LOCU_LIGHTING, gs_lighting);

	glDrawElements(GL_TRIANGLES, g_obj_earth.n_faces * 3, GL_UNSIGNED_INT, (GLvoid *)0);
}
