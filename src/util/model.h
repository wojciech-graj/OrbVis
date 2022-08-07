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
