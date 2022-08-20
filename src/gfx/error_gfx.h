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

#ifndef __ERROR_GFX_H__
#define __ERROR_GFX_H__

#include "system.h"
#include "type.h"
#include <stdio.h>

extern char gfx_info_log[512];

#define gfx_error(func, arg, ...)                   \
	do {                                        \
		func(arg, 512, NULL, gfx_info_log); \
		printf_log(__VA_ARGS__);            \
		puts(gfx_info_log);                 \
		exit(1);                            \
		UNREACHABLE;                        \
	} while (0)

#define gfx_error_check(cond, func, arg, ...)              \
	do {                                               \
		if (unlikely(!(cond))) {                   \
			gfx_error(func, arg, __VA_ARGS__); \
		}                                          \
	} while (0)

#endif /* __ERROR_GFX_H__ */
