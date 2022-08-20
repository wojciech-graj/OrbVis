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

#ifndef __PERF_H__
#define __PERF_H__

#include "ui.h"

void perf_init(GtkBuilder *builder);

void perf_set_num_satellites(unsigned num_satellites);
void perf_tic(void);

void perf_show(void);
void perf_hide(void);

#endif /* __PERF_H__ */
