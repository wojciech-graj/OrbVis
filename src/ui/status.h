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
