#ifndef __SETTING_H__
#define __SETTING_H__

#include "type.h"
#include "ui.h"

extern bool gs_gmt;
extern bool gs_clouds;
extern bool gs_lighting;

void setting_init(GtkBuilder *builder);

#endif /* __SETTING_H__ */
