#ifndef __STATUS_H__
#define __STATUS_H__

#include "ui.h"

enum Status {
	STAT_FETCHING_SAT = 0u,
};

void status_init(GtkBuilder *builder);
void status_push(unsigned ctx_id, const char *text);
void status_pop(unsigned ctx_id);

#endif
