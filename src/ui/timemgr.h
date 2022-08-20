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

#ifndef __TIMEMGR_H__
#define __TIMEMGR_H__

#include "ui.h"

enum Timeflow {
	TIME_REALTIME = 0u,
	TIME_PAUSE,
	TIME_ARBITRARY,
};

extern enum Timeflow e_timeflow;
extern float e_timescale;

void timemgr_init(GtkBuilder *builder);
void timemgr_tic(void);

#endif /* __TIMEMGR_H__ */
