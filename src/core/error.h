#ifndef __ERROR_H__
#define __ERROR_H__

#include "type.h"

#include <stdlib.h>

#include "system.h"

#define error(...)                       \
	do {                             \
		printf_log(__VA_ARGS__); \
		exit(1);                 \
		UNREACHABLE;             \
	} while (0)

#define error_check(cond, ...)              \
	do {                                \
		if (unlikely(!(cond))) {    \
			error(__VA_ARGS__); \
		}                           \
	} while (0)

#endif /* __ERROR_H__ */
