#include "status.h"

GtkStatusbar *g_statusbar;

void status_init(GtkBuilder *builder)
{
	g_statusbar = GTK_STATUSBAR(gtk_builder_get_object(builder, "statusbar"));
}
