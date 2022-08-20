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

#ifndef __INFO_H__
#define __INFO_H__

#include "satellite.h"
#include "ui.h"

void info_init(GtkBuilder *builder);

void info_show(struct Satellite *satellite);
void info_hide(void);
void info_tic(void);

#endif /* __INFO_H__ */
