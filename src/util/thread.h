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

#endif /* __THREAD_H__ */
