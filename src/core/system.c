#include "system.h"

#include "error.h"

#include <glib.h>

static GDateTime *t_start;
static char iso8601_str[29];

void system_init(void)
{
	t_start = g_date_time_new_now_utc();
}

void system_deinit(void)
{
	g_date_time_unref(t_start);
}

double system_time(void)
{
	GDateTime *t_now = g_date_time_new_now_utc();
	GTimeSpan diff = g_date_time_difference(t_now, t_start);
	g_date_time_unref(t_now);
	return diff * 1.e-6;
}

long system_epoch_ms(void)
{
	GDateTime *t_now = g_date_time_new_now_utc();
	long epoch_ms = 1000L * g_date_time_to_unix(t_now) + g_date_time_get_microsecond(t_now) / 1000L;
	g_date_time_unref(t_now);
	return epoch_ms;
}

char *epoch_to_iso8601(long epoch_ms, gboolean gmt, gboolean ms)
{
	long epoch_s = epoch_ms / 1000L;
	GDateTime *t_now = (gmt) ? g_date_time_new_from_unix_utc(epoch_s) : g_date_time_new_from_unix_local(epoch_s);
	gchar *str = g_date_time_format(t_now, "%FT%T");
	memcpy(iso8601_str, str, 19);
	g_free(str);
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
		str = g_date_time_format(t_now, "%z");
		memcpy(iso8601_str + str_idx, str, 6);
		g_free(str);
	}
	g_date_time_unref(t_now);
	return iso8601_str;
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
