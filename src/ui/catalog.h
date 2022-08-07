#ifndef __CATALOG_H__
#define __CATALOG_H__

#include "satellite.h"
#include "ui.h"

void catalog_init(GtkBuilder *builder);
void catalog_satellite_changed(struct Satellite *satellite);
void catalog_construct_views(void);
void catalog_deconstruct_views(void);

extern GtkListStore *g_satellite_store;

#endif /* __CATALOG_H__ */
