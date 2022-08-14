#include "ui.h"

#include "catalog.h"
#include "entity.h"
#include "error.h"
#include "gfx.h"
#include "input.h"
#include "render.h"
#include "setting.h"
#include "status.h"
#include "system.h"
#include "thread.h"
#include "timemgr.h"
#include "info.h"

const gchar *FILENAME_GUI = "res/ui/ui.glade";

GtkWindow *e_window_main;

static void on_window_main_destroy(GtkWidget *widget, gpointer user_data);
static gboolean on_window_main_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);

gboolean on_window_main_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	(void)widget;
	(void)event;
	(void)user_data;

	thread_deinit();
	render_deinit();
	entity_deinit();
	ui_deinit();
	system_deinit();

	return FALSE;
}

void on_window_main_destroy(GtkWidget *widget, gpointer user_data)
{
	(void)widget;
	(void)user_data;

	gtk_main_quit();
}

void ui_init(int argc, char ***argv)
{
	gtk_init(&argc, argv);

	GError *err = NULL;
	GtkBuilder *builder = gtk_builder_new();

	gtk_builder_add_from_file(builder, FILENAME_GUI, &err);
	error_check(!err, "Failed to open GLADE UI file");

	gtk_builder_add_callback_symbols(builder,
		"on_window_main_destroy", G_CALLBACK(on_window_main_destroy),
		"on_window_main_delete_event", G_CALLBACK(on_window_main_delete_event),
		NULL);

	GtkWidget *window_main = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
	e_window_main = GTK_WINDOW(window_main);

	gfx_init(builder);
	input_init(builder);
	status_init(builder);
	timemgr_init(builder);
	catalog_init(builder);
	setting_init(builder);
	info_init(builder);

	gtk_builder_connect_signals(builder, NULL);

	gtk_widget_show_all(window_main);

	g_object_unref((gpointer)builder);
}

void ui_deinit(void)
{
	catalog_deinit();
}
