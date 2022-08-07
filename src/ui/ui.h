#ifndef __UI_H__
#define __UI_H__

#include <gtk/gtk.h>

void ui_init(int argc, char ***argv);
void ui_deinit(void);

extern GtkWindow *g_window_main;

#endif /* __UI_H__ */
