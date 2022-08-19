#include "setting.h"

#include "perf.h"

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

static GtkWindow *window_settings;

static void on_settings_clicked(GtkToolButton *toolbutton, gpointer user_data);
static gboolean on_window_settings_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static void on_setting_toggled(GtkToggleButton *togglebutton, gpointer user_data);

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

void setting_init(GtkBuilder *builder)
{
	gtk_builder_add_callback_symbols(builder,
		"on_settings_clicked", G_CALLBACK(on_settings_clicked),
		"on_setting_toggled", G_CALLBACK(on_setting_toggled),
		"on_window_settings_delete_event", G_CALLBACK(on_window_settings_delete_event),
		NULL);

	window_settings = GTK_WINDOW(gtk_builder_get_object(builder, "window_settings"));

	unsigned i;
	for (i = 0; i < 3; i++)
		settings[i].button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, settings[i].id));
}
