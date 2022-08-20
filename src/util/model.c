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
