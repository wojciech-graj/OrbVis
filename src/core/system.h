#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "type.h"

#include <stdio.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

void system_init(void);
void system_deinit(void);

double system_time(void);
gint64 system_epoch_ms(void);
char *epoch_to_iso8601(gint64 epoch_ms, gboolean gmt, gboolean ms);
long fsize(FILE *file);
size_t count(char *str, char c, size_t size);

#pragma GCC system_header /* Ignore -Wpedantic warning about ##__VA_ARGS__ trailing comma */
#define printf_log(fmt, ...)                                               \
	do {                                                               \
		printf("[%08.3f] %10s:%18s:%3u: " fmt "\n", system_time(), \
			__FILENAME__, __func__, __LINE__, ##__VA_ARGS__);  \
	} while (0)

#endif /* __SYSTEM_H__ */
