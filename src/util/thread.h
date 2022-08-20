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

#ifndef __THREAD_H__
#define __THREAD_H__

#include <glib.h>

#include "type.h"

enum ThreadID {
	THRD_SATELLITES_GET = 0u,
	THRD_PHYS,
	NUM_THRDS,
};

struct Thread {
	GThread *thread;
	volatile gint finished;
	void *data;
};

void thread_deinit(void);

void thread_dispatch(enum ThreadID id, void *data);

/* Sync is executed after join */
void thread_join(enum ThreadID id);
void thread_join_if_finished(enum ThreadID id);

extern struct Thread e_threads[NUM_THRDS];

#endif /* __THREAD_H__ */
