/*
 * Copyright (c) 2022-2023 Wojciech Graj
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

#ifndef __TYPE_H__
#define __TYPE_H__

#include <glib.h>

#define UNREACHABLE __builtin_unreachable()

#define likely(x) G_LIKELY(x)
#define unlikely(x) G_UNLIKELY(x)

#define SWAP(a, b)                       \
	do {                             \
		__typeof__(a) temp_ = a; \
		a = b;                   \
		b = temp_;               \
	} while (0)

#endif /* __TYPE_H__ */
