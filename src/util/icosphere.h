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

#ifndef __ICOSPHERE_H__
#define __ICOSPHERE_H__

#include <cglm/vec2.h>
#include <cglm/vec3.h>

#include "type.h"

void icosphere_generate(const unsigned n_sub, vec3 **verts, guint32 **faces, vec2 **uv, size_t *n_verts, size_t *n_faces);

#endif /* __ICOSPHERE_H__ */
