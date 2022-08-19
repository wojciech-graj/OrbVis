#ifndef __PERF_H__
#define __PERF_H__

#include "ui.h"

void perf_init(GtkBuilder *builder);

void perf_set_num_satellites(unsigned num_satellites);
void perf_tic(void);

void perf_show(void);
void perf_hide(void);

#endif /* __PERF_H__ */
