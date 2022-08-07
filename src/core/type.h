#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define UNREACHABLE __builtin_unreachable()

#define likely(x) __builtin_expect((x), true)
#define unlikely(x) __builtin_expect((x), false)

#define arrlen(a) (sizeof(a) / sizeof(a[0]))

#define SWAP(a, b)                       \
	do {                             \
		__typeof__(a) temp_ = a; \
		a = b;                   \
		b = temp_;               \
	} while (0)

#define sgn(x) (signbit(x) ? -1 : 1)

#define PI 3.1415927f

#endif /* __TYPE_H__ */
