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

#include "thread.h"

#include "error.h"
#include "phys.h"
#include "satellite.h"

struct ThreadParams {
	void (*prep)(void);
	void *(*start_routine)(void *);
	void (*sync)(void);
};

struct Thread e_threads[NUM_THRDS] = {
	[THRD_SATELLITES_GET] = { 0 },
	[THRD_PHYS] = { 0 },
};

static const struct ThreadParams params[NUM_THRDS] = {
	[THRD_SATELLITES_GET] = {
		.prep = &satellites_get_prep,
		.start_routine = &satellites_get_thrd,
		.sync = &satellites_get_sync,
	},
	[THRD_PHYS] = {
		.prep = NULL,
		.start_routine = &phys_thrd,
		.sync = &phys_sync,
	},
};

void thread_dispatch(enum ThreadID id, void *data)
{
	if (params[id].prep)
		params[id].prep();

	e_threads[id].data = data;
	e_threads[id].thread = g_thread_new(NULL, params[id].start_routine, &e_threads[id]);
}

void thread_join(enum ThreadID id)
{
	g_thread_join(e_threads[id].thread);
	g_atomic_int_set(&e_threads[id].finished, 0);
	e_threads[id].thread = NULL;
	params[id].sync();
}

void thread_join_if_finished(enum ThreadID id)
{
	if (g_atomic_int_get(&e_threads[id].finished))
		thread_join(id);
}

void thread_deinit(void)
{
	/* TODO: Kill any running threads */
}
