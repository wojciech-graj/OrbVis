#include "system.h"

#include <stdlib.h>
#include <time.h>

#include "error.h"

static struct timespec start_t;
static char iso8601_str[29];

void system_init(void)
{
	timespec_get(&start_t, TIME_UTC);
	srand((unsigned)start_t.tv_sec);
}

double system_time(void)
{
	struct timespec cur_t;
	timespec_get(&cur_t, TIME_UTC);
	return (cur_t.tv_sec - start_t.tv_sec) + (cur_t.tv_nsec - start_t.tv_nsec) * 1e-9;
}

long system_epoch_ms(void)
{
	struct timespec cur_t;
	timespec_get(&cur_t, TIME_UTC);
	return 1000L * cur_t.tv_sec + cur_t.tv_nsec / 1000000L;
}

char *epoch_to_iso8601(long epoch_ms, bool gmt, bool ms)
{
	time_t epoch_s = epoch_ms / 1000l;
	struct tm tm_s;
	if (gmt)
		gmtime_r(&epoch_s, &tm_s);
	else
		localtime_r(&epoch_s, &tm_s);
	strftime(iso8601_str, 20, "%FT%T", &tm_s);
	unsigned str_idx;
	if (ms) {
		sprintf(iso8601_str + 19, ".%03ld", epoch_ms % 1000);
		str_idx = 23;
	} else {
		str_idx = 19;
	}
	if (gmt) {
		iso8601_str[str_idx] = 'Z';
		iso8601_str[str_idx + 1] = '\0';
	} else {
		strftime(iso8601_str + str_idx, 6, "%z", &tm_s);
	}
	return iso8601_str;
}

float rand_flt(void)
{
	return rand() / (float)RAND_MAX;
}

long fsize(FILE *file)
{
	long len;
	if (fseek(file, 0, SEEK_END))
		return -1;
	len = ftell(file);
	if (fseek(file, 0, SEEK_SET))
		return -1;
	return len;
}

size_t count(char *str, char c, size_t size)
{
	size_t cnt = 0;
	size_t i;
	char *p = str;
	for (i = 0; i < size; i++) {
		if (*p == c)
			cnt++;
		p++;
	}
	return cnt;
}
