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

#include "setting.h"

#include "perf.h"
#include "satellite.h"
#include "thread.h"
#include "version.h"

#include <stdio.h>

struct Setting {
	char *id;
	GtkToggleButton *button;
	gboolean *val;
};

struct Setting settings[] = {
	{
		.id = "lighting",
		.val = &gs_lighting,
	},
	{
		.id = "clouds",
		.val = &gs_clouds,
	},
	{
		.id = "local_time",
		.val = &gs_localtime,
	},
	{
		.id = "invert_scroll",
		.val = &gs_invert_scroll,
	},
};

gboolean gs_localtime = TRUE;
gboolean gs_clouds = TRUE;
gboolean gs_lighting = TRUE;
gboolean gs_invert_scroll = FALSE;
enum ReferenceFrame gs_reference_frame = REFERENCE_FRAME_FIXED;

static const char *SETTINGS_FILENAME = ".orbvis.conf";
static gchar *settings_filepath;

static GtkWindow *window_settings;

static void on_settings_clicked(GtkToolButton *toolbutton, gpointer user_data);
static gboolean on_window_settings_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static void on_setting_toggled(GtkToggleButton *togglebutton, gpointer user_data);
static void on_fetch_data_clicked(GtkButton *button, gpointer user_data);
static gboolean on_reference_frame_state_set(GtkSwitch *widget, gboolean state, gpointer user_data);

static void settings_load(void);
static void settings_save(void);

void on_settings_clicked(GtkToolButton *toolbutton, gpointer user_data)
{
	(void)toolbutton;
	(void)user_data;
	gtk_widget_show_all(GTK_WIDGET(window_settings));
	perf_show();
}

gboolean on_window_settings_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	(void)widget;
	(void)event;
	(void)user_data;
	gtk_widget_hide(GTK_WIDGET(window_settings));
	perf_hide();
	return TRUE;
}

void on_setting_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	(void)user_data;
	unsigned i;
	for (i = 0; i < 4 && settings[i].button != togglebutton; i++)
		;
	*settings[i].val = gtk_toggle_button_get_active(togglebutton);
}

void on_fetch_data_clicked(GtkButton *button, gpointer user_data)
{
	(void)button;
	(void)user_data;
	if (!e_threads[THRD_SATELLITES_GET].thread) {
		satellite_clear_cache();
		thread_dispatch(THRD_SATELLITES_GET, NULL);
	}
}

gboolean on_reference_frame_state_set(GtkSwitch *widget, gboolean state, gpointer user_data)
{
	(void)widget;
	(void)user_data;

	gs_reference_frame = (enum ReferenceFrame)state;

	return FALSE;
}

void settings_save(void)
{
	g_autoptr(GKeyFile) settings_file = g_key_file_new();
	unsigned i;
	for (i = 0; i < 4; i++)
		g_key_file_set_boolean(settings_file, "settings", settings[i].id, *settings[i].val);
	g_key_file_save_to_file(settings_file, settings_filepath, NULL);
}

void settings_load(void)
{
	settings_filepath = g_build_filename(g_get_user_config_dir(), SETTINGS_FILENAME, NULL);
	g_autoptr(GKeyFile) settings_file = g_key_file_new();
	if (g_key_file_load_from_file(settings_file, settings_filepath, G_KEY_FILE_NONE, NULL)) {
		unsigned i;
		for (i = 0; i < 4; i++) {
			g_autoptr(GError) error = NULL;
			gboolean val = g_key_file_get_boolean(settings_file, "settings", settings[i].id, &error);
			if (!error)
				*settings[i].val = val;
		}
	}
}

void setting_init(GtkBuilder *builder)
{
	gtk_builder_add_callback_symbols(builder,
		"on_settings_clicked", G_CALLBACK(on_settings_clicked),
		"on_setting_toggled", G_CALLBACK(on_setting_toggled),
		"on_window_settings_delete_event", G_CALLBACK(on_window_settings_delete_event),
		"on_fetch_data_clicked", G_CALLBACK(on_fetch_data_clicked),
		"on_reference_frame_state_set", G_CALLBACK(on_reference_frame_state_set),
		NULL);

	window_settings = GTK_WINDOW(gtk_builder_get_object(builder, "window_settings"));

	GtkLabel *about = GTK_LABEL(gtk_builder_get_object(builder, "about"));
	gtk_label_set_text(about, ABOUT_STRING);

	settings_load();

	unsigned i;
	for (i = 0; i < 4; i++) {
		settings[i].button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, settings[i].id));
		gtk_toggle_button_set_active(settings[i].button, *settings[i].val);
	}
}

void setting_deinit(void)
{
	settings_save();
	g_free(settings_filepath);
}
