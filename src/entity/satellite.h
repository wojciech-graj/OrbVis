/*
 * Copyright (c) 2022 Wojciech Graj
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

#ifndef __SATELLITE_H__
#define __SATELLITE_H__

#include "TLE.h"
#include "satcat.h"
#include "type.h"

struct Satellite {
	char name[25];
	TLE tle;
	struct SatCat satcat;
	guint32 orbit_idx;
};

void satellite_init(void);
void satellite_deinit(void);
void satellites_render(void);

void satellites_phys(void);
void satellites_phys_sync(void);

void satellites_tic(void);
void satellites_tic_sync(void);

void satellites_get_prep(void);
void *satellites_get_thrd(void *arguments);
void satellites_get_sync(void);

void satellite_select(double xpos, double ypos);
void satellite_select_ptr(struct Satellite *satellite);

void satellite_clear_cache(void);

void satellites_filter(void);

#endif /* __SATELLITE_H__ */
