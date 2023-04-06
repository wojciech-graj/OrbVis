/*
 * Copyright (c) 2022-2023 Wojciech Graj
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
		.val = &es_lighting,
	},
	{
		.id = "clouds",
		.val = &es_clouds,
	},
	{
		.id = "local_time",
		.val = &es_localtime,
	},
	{
		.id = "invert_scroll",
		.val = &es_invert_scroll,
	},
};

gboolean es_localtime = TRUE;
gboolean es_clouds = TRUE;
gboolean es_lighting = TRUE;
gboolean es_invert_scroll = FALSE;
enum ReferenceFrame gs_reference_frame = REFERENCE_FRAME_FIXED;

static const char *SETTINGS_FILENAME = ".orbvis.conf";
static gchar *settings_filepath;

static GtkWindow *window_settings;
static GtkSwitch *switch_reference_frame;

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
	GKeyFile *settings_file = g_key_file_new();
	unsigned i;
	for (i = 0; i < 4; i++)
		g_key_file_set_boolean(settings_file, "settings", settings[i].id, *settings[i].val);
	g_key_file_set_integer(settings_file, "settings", "reference_frame", gs_reference_frame);
	g_key_file_save_to_file(settings_file, settings_filepath, NULL);
	g_key_file_free(settings_file);
}

void settings_load(void)
{
	settings_filepath = g_build_filename(g_get_user_config_dir(), SETTINGS_FILENAME, NULL);
	GKeyFile *settings_file = g_key_file_new();
	if (g_key_file_load_from_file(settings_file, settings_filepath, G_KEY_FILE_NONE, NULL)) {
		unsigned i;
		for (i = 0; i < 4; i++) {
			GError *err = NULL;
			gboolean val = g_key_file_get_boolean(settings_file, "settings", settings[i].id, &err);
			if (err)
				g_error_free(err);
			else
				*settings[i].val = val;
		}
	}
	GError *err = NULL;
	gint reference_frame = g_key_file_get_integer(settings_file, "settings", "reference_frame", &err);
	if (err)
		g_error_free(err);
	else
		gs_reference_frame = (enum ReferenceFrame)reference_frame;
	g_key_file_free(settings_file);

	unsigned i;
	for (i = 0; i < 4; i++)
		gtk_toggle_button_set_active(settings[i].button, *settings[i].val);
	gtk_switch_set_state(switch_reference_frame, gs_reference_frame);
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
	switch_reference_frame = GTK_SWITCH(gtk_builder_get_object(builder, "reference_frame"));

	GtkLabel *about = GTK_LABEL(gtk_builder_get_object(builder, "about"));
	gtk_label_set_text(about, ABOUT_STRING);

	unsigned i;
	for (i = 0; i < 4; i++)
		settings[i].button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, settings[i].id));

	settings_load();
}

void setting_deinit(void)
{
	settings_save();
	g_free(settings_filepath);
}
