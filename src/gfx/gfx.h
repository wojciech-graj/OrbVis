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

#ifndef __GFX_H__
#define __GFX_H__

#include <epoxy/gl.h>

#include "ui.h"

struct GLCtx {
	unsigned res_x;
	unsigned res_y;
};

void gfx_init(GtkBuilder *builder);

extern struct GLCtx e_gl_ctx;

#endif /* __GFX_H__ */
