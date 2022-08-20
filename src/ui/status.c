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

#include "status.h"

static GtkStatusbar *statusbar;

void status_init(GtkBuilder *builder)
{
	statusbar = GTK_STATUSBAR(gtk_builder_get_object(builder, "statusbar"));
}

void status_push(unsigned ctx_id, const char *text)
{
	gtk_statusbar_push(statusbar, ctx_id, text);
}

void status_pop(unsigned ctx_id)
{
	gtk_statusbar_pop(statusbar, ctx_id);
}
