#ifndef __SHADER_H__
#define __SHADER_H__

#include "gfx.h"

struct ShaderAttr {
	GLuint index;
	const GLchar *name;
};

struct Shader {
	GLuint handle;
};

void shader_init(struct Shader *shader, char *vs_path, char *fs_path, size_t n_vertex_attr, struct ShaderAttr vertex_attr[]);
void shader_deinit(struct Shader *shader);
void shader_bind(struct Shader *shader);

#endif /* __SHADER_H__ */
