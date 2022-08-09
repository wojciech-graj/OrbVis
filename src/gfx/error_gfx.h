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
