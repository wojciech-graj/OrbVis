#ifndef __INFO_H__
#define __INFO_H__

#include "ui.h"
#include "satellite.h"

void info_init(GtkBuilder *builder);

void info_show(struct Satellite *satellite);
void info_hide(void);
void info_tic(void);

#endif /* __INFO_H__ */