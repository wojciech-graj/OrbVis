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

#ifndef __MODEL_H__
#define __MODEL_H__

#include <cglm/mat4.h>

struct Model {
	struct IdxObj *obj;
	mat4 trans;
	mat4 rot;
	mat4 scl;
	mat4 model_mat;
};

void model_init(struct Model *model, struct IdxObj *obj);
void model_transform(struct Model *model);

#endif /* __MODEL_H__ */
