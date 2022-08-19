#ifndef __CATALOG_H__
#define __CATALOG_H__

#include "satellite.h"
#include "ui.h"

void catalog_init(GtkBuilder *builder);
void catalog_deinit(void);
void catalog_satellite_changed(struct Satellite *satellite);
void catalog_satellites_fill(struct Satellite *satellites, size_t n_satellites);

extern GtkTreeModelFilter *e_catalog_filter;

#endif /* __CATALOG_H__ */
