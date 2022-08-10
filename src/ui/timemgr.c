#include "timemgr.h"

#include "phys.h"
#include "setting.h"
#include "system.h"

#include <stdio.h>

enum Timeflow e_timeflow = TIME_REALTIME;
float e_timescale = 1.f;

static GtkToggleToolButton *play_button;
static GtkEntry *time_entry;
static GtkEntry *speed_entry;

static void on_play_toggled(GtkToggleToolButton *toggle_tool_button, gpointer user_data);
static void on_realtime_clicked(GtkToolButton *toolbutton, gpointer user_data);
static void on_decelerate_clicked(GtkToolButton *toolbutton, gpointer user_data);
static void on_accelerate_clicked(GtkToolButton *toolbutton, gpointer user_data);
static void on_time_activate(GtkEntry *entry, gpointer user_data);
static void on_speed_activate(GtkEntry *entry, gpointer user_data);
static void timeflow_set(enum Timeflow timeflow);

void on_play_toggled(GtkToggleToolButton *toggle_tool_button, gpointer user_data)
{
	(void)user_data;
	if (!gtk_toggle_tool_button_get_active(toggle_tool_button))
		e_timeflow = TIME_PAUSE;
	else if (e_timeflow == TIME_PAUSE)
		e_timeflow = TIME_ARBITRARY;
}

void on_realtime_clicked(GtkToolButton *toolbutton, gpointer user_data)
{
	(void)toolbutton;
	(void)user_data;
	timeflow_set(TIME_REALTIME);
	e_timescale = 1.f;
}

void on_decelerate_clicked(GtkToolButton *toolbutton, gpointer user_data)
{
	(void)toolbutton;
	(void)user_data;
	if (e_timeflow == TIME_REALTIME)
		timeflow_set(TIME_ARBITRARY);
	e_timescale *= 0.95f;
}

void on_accelerate_clicked(GtkToolButton *toolbutton, gpointer user_data)
{
	(void)toolbutton;
	(void)user_data;
	if (e_timeflow == TIME_REALTIME)
		timeflow_set(TIME_ARBITRARY);
	e_timescale *= 1.05f;
}

void on_time_activate(GtkEntry *entry, gpointer user_data)
{
	(void)entry;
	(void)user_data;

	gtk_window_set_focus(e_window_main, NULL);

	GDateTime *dt = g_date_time_new_from_iso8601(gtk_entry_get_text(time_entry), NULL);
	if (!dt)
		return;

	e_phys.epoch_ms = 1000LL * g_date_time_to_unix(dt) + g_date_time_get_microsecond(dt) / 1000LL;
	g_date_time_unref(dt);
	timeflow_set(TIME_PAUSE);
}

void on_speed_activate(GtkEntry *entry, gpointer user_data)
{
	(void)entry;
	(void)user_data;

	gtk_window_set_focus(e_window_main, NULL);

	const gchar *string = gtk_entry_get_text(speed_entry);
	char *end;
	float speed = strtof(string, &end);
	if (end == string)
		return;

	if (e_timeflow == TIME_REALTIME)
		timeflow_set(TIME_ARBITRARY);

	e_timescale = speed;
}

void timeflow_set(enum Timeflow timeflow)
{
	e_timeflow = timeflow;
	gtk_toggle_tool_button_set_active(play_button, (timeflow == TIME_PAUSE) ? FALSE : TRUE);
}

void timemgr_init(GtkBuilder *builder)
{
	gtk_builder_add_callback_symbols(builder,
		"on_play_toggled", G_CALLBACK(on_play_toggled),
		"on_realtime_clicked", G_CALLBACK(on_realtime_clicked),
		"on_decelerate_clicked", G_CALLBACK(on_decelerate_clicked),
		"on_accelerate_clicked", G_CALLBACK(on_accelerate_clicked),
		"on_time_activate", G_CALLBACK(on_time_activate),
		"on_speed_activate", G_CALLBACK(on_speed_activate),
		NULL);
	play_button = GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object(builder, "play"));
	time_entry = GTK_ENTRY(gtk_builder_get_object(builder, "time"));
	speed_entry = GTK_ENTRY(gtk_builder_get_object(builder, "speed"));
}

void timemgr_tic(void)
{
	if (!gtk_widget_has_focus(GTK_WIDGET(time_entry)))
		gtk_entry_set_text(time_entry, epoch_to_iso8601(e_phys.epoch_ms, gs_gmt, TRUE));
	if (!gtk_widget_has_focus(GTK_WIDGET(speed_entry))) {
		char buf[16];
		snprintf(buf, 16, "%.3fx", (double)e_timescale);
		gtk_entry_set_text(speed_entry, buf);
	}
}
