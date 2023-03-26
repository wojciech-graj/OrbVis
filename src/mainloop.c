/*
 * Copyright (c) 2023 Wojciech Graj
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

#include "mainloop.h"

#include "info.h"
#include "perf.h"
#include "render.h"
#include "satellite.h"
#include "thread.h"
#include "toolbar.h"

void mainloop(void)
{
	thread_dispatch(THRD_PHYS, NULL);
	satellites_tic();
	render_process();
	toolbar_tic();
	info_tic();
	perf_tic();
	thread_join(THRD_PHYS);
	satellites_tic_sync();
	thread_join_if_finished(THRD_SATELLITES_GET);
}
