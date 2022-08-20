/*
 * Copyright (c) 2022 Wojciech Graj
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 **/

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
