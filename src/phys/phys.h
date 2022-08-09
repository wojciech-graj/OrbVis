#ifndef __PHYS_H__
#define __PHYS_H__

#include "type.h"

#include <cglm/vec2.h>
#include <cglm/vec3.h>

#define MS_IN_DAY 86400000

struct PhysCtx {
	gint64 epoch_ms;
	double gmst;
	vec2 sun_uv;
	vec3 sun_dir;
};

extern struct PhysCtx g_phys;

void phys_init(void);
void *phys_thrd(void *arguments);
void phys_sync(void);

#endif
