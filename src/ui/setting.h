#ifndef __SETTING_H__
#define __SETTING_H__

#include "type.h"
#include "ui.h"

extern gboolean gs_gmt;
extern gboolean gs_clouds;
extern gboolean gs_lighting;

void setting_init(GtkBuilder *builder);

#endif /* __SETTING_H__ */
