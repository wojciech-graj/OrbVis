#ifndef __FILTER_H__
#define __FILTER_H__

#include "satellite.h"
#include "ui.h"

void filter_init(GtkBuilder *builder);
gboolean filter_func(struct Satellite *satellite);

#endif /* __FILTER_H__ */
