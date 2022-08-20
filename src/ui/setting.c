#include "setting.h"

#include "perf.h"
#include "satellite.h"
#include "thread.h"

struct Setting {
	char *id;
	GtkToggleButton *button;
	gboolean *val;
};

struct Setting settings[3] = {
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
		.val = &gs_gmt,
	},
};

gboolean gs_gmt = FALSE;
gboolean gs_clouds = TRUE;
gboolean gs_lighting = TRUE;
enum ReferenceFrame gs_reference_frame = REFERENCE_FRAME_FIXED;

static GtkWindow *window_settings;

static void on_settings_clicked(GtkToolButton *toolbutton, gpointer user_data);
static gboolean on_window_settings_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static void on_setting_toggled(GtkToggleButton *togglebutton, gpointer user_data);
static void on_fetch_data_clicked(GtkButton *button, gpointer user_data);
static gboolean on_reference_frame_state_set(GtkSwitch *widget, gboolean state, gpointer user_data);

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
	for (i = 0; i < 3 && settings[i].button != togglebutton; i++)
		;
	*settings[i].val = !*settings[i].val;
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

	unsigned i;
	for (i = 0; i < 3; i++)
		settings[i].button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, settings[i].id));
}
