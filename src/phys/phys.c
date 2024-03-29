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

#include "phys.h"

#include "SGP4.h"

#include "camera.h"
#include "satellite.h"
#include "setting.h"
#include "system.h"
#include "thread.h"

#include <cglm/mat3.h>

enum Timeflow e_timeflow = TIME_REALTIME;
float e_timescale = 1.f;
struct PhysCtx e_phys;
static gint64 epoch_ms;
static struct PhysCtx phys_ctx_sync;

static void phys(void);
static void phys_phys(void);
static void phys_phys_sync(void);

void phys_phys(void)
{
	gint64 cur_epoch_ms = system_epoch_ms();
	switch (e_timeflow) {
	case TIME_REALTIME:
		phys_ctx_sync.epoch_ms = cur_epoch_ms;
		break;
	case TIME_PAUSE:
		phys_ctx_sync.epoch_ms = e_phys.epoch_ms;
		break;
	case TIME_ARBITRARY:
		phys_ctx_sync.epoch_ms = e_phys.epoch_ms + (gint64)((cur_epoch_ms - epoch_ms) * e_timescale);
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
			      ((float)asin(0.39779 * cos(0.017203 * (n + 10) + 0.0334 * sin(0.017203 * (n - 2)))) + (float)G_PI) / (2.f * (float)G_PI),
		      },
		phys_ctx_sync.sun_uv);
	glm_vec3_copy((vec3){
			      -cosf(phys_ctx_sync.sun_uv[0] * 2.f * (float)G_PI) * sinf(phys_ctx_sync.sun_uv[1] * (float)G_PI),
			      -sinf(phys_ctx_sync.sun_uv[0] * 2.f * (float)G_PI) * sinf(phys_ctx_sync.sun_uv[1] * (float)G_PI),
			      cosf(phys_ctx_sync.sun_uv[1] * (float)G_PI),
		      },
		phys_ctx_sync.sun_dir);

	mat3 t;
	glm_vec3_copy((vec3){ 0.f, 0.f, 1.f }, t[2]);
	glm_vec3_copy((vec3){ (float)cos(phys_ctx_sync.gmst), (float)-sin(phys_ctx_sync.gmst), 0.f }, t[0]);
	glm_vec3_cross(t[2], t[0], t[1]);
	glm_vec3_norm(t[1]);
	glm_mat3_scale(t, 1.f / 6371.f);
	glm_mat4_ins3(t, phys_ctx_sync.teme_to_world);
}

void phys_init(void)
{
	glm_mat4_identity(phys_ctx_sync.teme_to_world);
	phys_phys();
	phys_phys_sync();
}

void phys(void)
{
	phys_phys();
	satellites_phys();
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
	if (es_reference_frame == REFERENCE_FRAME_INERTIAL) {
		gint64 diff_ms = phys_ctx_sync.epoch_ms - e_phys.epoch_ms;
		float dang = (-2.f * (float)G_PI * diff_ms) / MS_IN_DAY;
		glm_vec2_copy((vec2){ e_camera.pos[0] * cosf(dang) - e_camera.pos[1] * sinf(dang),
				      e_camera.pos[1] * cosf(dang) + e_camera.pos[0] * sinf(dang) },
			e_camera.pos);
		camera_view_update(&e_camera);
	}
	e_phys = phys_ctx_sync;
}

void phys_sync(void)
{
	phys_phys_sync();
	satellites_phys_sync();
}
