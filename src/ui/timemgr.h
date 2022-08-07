#ifndef __TIMEMGR_H__
#define __TIMEMGR_H__

#include "ui.h"

enum Timeflow {
	TIME_REALTIME = 0u,
	TIME_PAUSE,
	TIME_ARBITRARY,
};

extern enum Timeflow g_timeflow;
extern float g_timescale;

void timemgr_init(GtkBuilder *builder);
void timemgr_tic(void);

#endif /* __TIMEMGR_H__ */
