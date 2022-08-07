#include "model.h"

void model_init(struct Model *model, struct IdxObj *obj)
{
	model->obj = obj;

	glm_mat4_identity(model->trans);
	glm_mat4_identity(model->rot);
	glm_mat4_identity(model->scl);
	glm_mat4_identity(model->model_mat);
}

void model_transform(struct Model *model)
{
	glm_mat4_mulN((mat4 *[]){ &model->trans, &model->rot, &model->scl }, 3, model->model_mat);
}
