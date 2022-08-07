#include "phys.h"

#include "SGP4.h"

#include "satellite.h"
#include "system.h"
#include "thread.h"
#include "timemgr.h"

#include <math.h>

struct PhysCtx g_phys;
static long epoch_ms;
static struct PhysCtx phys_ctx_sync;

static void phys(void);
static void phys_phys(void);
static void phys_phys_sync(void);

void phys_phys(void)
{
	long cur_epoch_ms = system_epoch_ms();
	switch (g_timeflow) {
	case TIME_REALTIME:
		phys_ctx_sync.epoch_ms = cur_epoch_ms;
		break;
	case TIME_PAUSE:
		phys_ctx_sync.epoch_ms = g_phys.epoch_ms;
		break;
	case TIME_ARBITRARY:
		phys_ctx_sync.epoch_ms = g_phys.epoch_ms + (long)((cur_epoch_ms - epoch_ms) * g_timescale);
		break;
	}
	epoch_ms = cur_epoch_ms;

	double jd_epoch = phys_ctx_sync.epoch_ms / (double)MS_IN_DAY;
	/* double j2000 = jd_epoch - 10957.5; */
	double julian = jd_epoch + 2440587.5;
	phys_ctx_sync.gmst = gstime(julian);

	double temp;
	double n = fmod(jd_epoch, 365.25); /* Days elapsed since Jan 1 Midnight */
	glm_vec2_copy((vec2){
			      1.0 - modf(jd_epoch, &temp), /* Omitted Equation of Time */
			      ((float)asin(0.39779 * cos(0.017203 * (n + 10) + 0.0334 * sin(0.017203 * (n - 2)))) + PI) / (2.f * PI),
		      },
		phys_ctx_sync.sun_uv);
	glm_vec3_copy((vec3){
			      -cosf(phys_ctx_sync.sun_uv[0] * 2.f * PI) * sinf(phys_ctx_sync.sun_uv[1] * PI),
			      -sinf(phys_ctx_sync.sun_uv[0] * 2.f * PI) * sinf(phys_ctx_sync.sun_uv[1] * PI),
			      cosf(phys_ctx_sync.sun_uv[1] * PI),
		      },
		phys_ctx_sync.sun_dir);
}

void phys_init(void)
{
	phys_phys();
	phys_phys_sync();
}

static void phys(void)
{
	phys_phys();
#ifndef NO_SATELLITE
	satellites_phys();
#endif
}

void *phys_thrd(void *arguments)
{
	struct Thread *thread = (struct Thread *)arguments;
	phys();
	g_atomic_int_set(&thread->finished, 1);
	return NULL;
}

void phys_phys_sync(void)
{
	g_phys = phys_ctx_sync;
}

void phys_sync(void)
{
	phys_phys_sync();
#ifndef NO_SATELLITE
	satellites_phys_sync();
#endif
}
