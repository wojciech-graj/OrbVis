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

	shader_init(&shader, "res/shader/sky.vert", "res/shader/sky.frag", 2, (struct ShaderAttr[]){
										      { LOCL_APOS, "in_pos" },
										      { LOCL_ATEXCOORD, "in_uv" },
									      });

	texture_init_from_image(&texture, "res/texture/8k_stars.jpg", GL_TEXTURE0, GL_TEXTURE_2D);

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
