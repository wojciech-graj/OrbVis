#include "gfx.h"

#include "camera.h"
#include "error.h"
#include "render.h"
#include "thread.h"
#include "timemgr.h"

struct GLCtx g_gl_ctx;
static GtkGLArea *glarea;

static void on_glarea_realize(GtkWidget *widget, gpointer user_data);
static void on_glarea_resize(GtkGLArea *area, int width, int height, gpointer user_data);
static gboolean on_glarea_render(GtkGLArea *area, GdkGLContext *context, gpointer user_data);

void on_glarea_realize(GtkWidget *widget, gpointer user_data)
{
	(void)user_data;
	(void)widget;

	error_check(!gtk_gl_area_get_error(glarea), "Failed to create OpenGL context");

	gtk_gl_area_make_current(glarea);

	GdkGLContext *glcontext = gtk_gl_area_get_context(glarea);
	GdkWindow *glwindow = gdk_gl_context_get_window(glcontext);
	GdkFrameClock *frame_clock = gdk_window_get_frame_clock(glwindow);

	g_signal_connect_swapped(frame_clock, "update", G_CALLBACK(gtk_gl_area_queue_render), glarea);

	gdk_frame_clock_begin_updating(frame_clock);
}

void on_glarea_resize(GtkGLArea *area, int width, int height, gpointer user_data)
{
	(void)area;
	(void)user_data;
	g_gl_ctx.res_x = width;
	g_gl_ctx.res_y = height;
	glViewport(0, 0, width, height);
	camera_proj_update(&g_camera);
}

/* Main Loop */
gboolean on_glarea_render(GtkGLArea *area, GdkGLContext *context, gpointer user_data)
{
	(void)area;
	(void)context;
	(void)user_data;

	thread_dispatch(THRD_PHYS, NULL);
	render_process();
	timemgr_tic();
#ifndef NO_SATELLITE
	thread_join_if_finished(THRD_SATELLITES_GET);
#endif
	thread_join(THRD_PHYS);

	return TRUE;
}

void gfx_init(GtkBuilder *builder)
{
	gtk_builder_add_callback_symbols(builder,
		"on_glarea_render", G_CALLBACK(on_glarea_render),
		"on_glarea_resize", G_CALLBACK(on_glarea_resize),
		"on_glarea_realize", G_CALLBACK(on_glarea_realize),
		NULL);

	glarea = GTK_GL_AREA(gtk_builder_get_object(builder, "glarea"));
	gtk_gl_area_set_required_version(glarea, 4, 6);
}
