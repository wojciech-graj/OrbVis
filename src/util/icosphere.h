#ifndef __ICOSPHERE_H__
#define __ICOSPHERE_H__

#include <cglm/vec2.h>
#include <cglm/vec3.h>

#include <stdint.h>

void icosphere_generate(const unsigned n_sub, vec3 **verts, uint32_t **faces, vec2 **uv, size_t *n_verts, size_t *n_faces);

#endif /* __ICOSPHERE_H__ */
