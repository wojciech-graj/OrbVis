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
