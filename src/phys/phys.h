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

#ifndef __PHYS_H__
#define __PHYS_H__

#include "type.h"

#include <cglm/vec2.h>
#include <cglm/vec3.h>

#define MS_IN_DAY 86400000

struct PhysCtx {
	gint64 epoch_ms;
	double gmst;
	vec2 sun_uv;
	vec3 sun_dir;
};

extern struct PhysCtx e_phys;

void phys_init(void);
void *phys_thrd(void *arguments);
void phys_sync(void);

#endif
