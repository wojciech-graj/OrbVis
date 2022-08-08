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
