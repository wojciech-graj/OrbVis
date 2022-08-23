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

#include "perf.h"

#include "satellite.h"
#include "system.h"

#define NUM_FRAMETIMES 30

static gboolean visible = FALSE;

static GtkLabel *fps_lbl;
static GtkLabel *num_satellites_lbl;

static gint64 prev_epoch_ms;
static int frametimes[NUM_FRAMETIMES] = { 0 };
static int frametime_total = 0;
static unsigned frame_idx = 0;

void perf_init(GtkBuilder *builder)
{
	fps_lbl = GTK_LABEL(gtk_builder_get_object(builder, "fps"));
	num_satellites_lbl = GTK_LABEL(gtk_builder_get_object(builder, "num_satellites"));
}

void perf_set_num_satellites(unsigned num_satellites)
{
	gchar *text = g_strdup_printf("%u", num_satellites);
	gtk_label_set_text(num_satellites_lbl, text);
	g_free(text);
}

void perf_show(void)
{
	visible = TRUE;
}

void perf_hide(void)
{
	visible = FALSE;
}

void perf_tic(void)
{
	frametime_total -= frametimes[frame_idx];
	gint64 epoch_ms = system_epoch_ms();
	frametimes[frame_idx] = epoch_ms - prev_epoch_ms;
	prev_epoch_ms = epoch_ms;
	frametime_total += frametimes[frame_idx];
	frame_idx = (frame_idx + 1) % NUM_FRAMETIMES;

	if (visible) {
		gchar *text = g_strdup_printf("%.1f", (NUM_FRAMETIMES * 1000.0) / frametime_total);
		gtk_label_set_text(fps_lbl, text);
		g_free(text);
	}
}
