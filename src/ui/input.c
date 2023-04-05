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

#include "input.h"

#include "camera.h"
#include "gfx.h"
#include "satellite.h"
#include "search.h"
#include "setting.h"
#include "type.h"
#include "ui.h"

#include <cglm/affine.h>
#include <cglm/mat4.h>
#include <cglm/vec3.h>

static double cursor_press_xpos;
static double cursor_press_ypos;

static double cursor_xpos;
static double cursor_ypos;

static gboolean cursor_pressed = FALSE;

static gboolean on_glarea_scroll_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static gboolean on_glarea_motion_notify_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static gboolean on_glarea_button_press_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static gboolean on_glarea_button_release_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static gboolean on_glarea_key_press_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);

void input_init(GtkBuilder *builder)
{
	gtk_builder_add_callback_symbols(builder,
		"on_glarea_scroll_event", G_CALLBACK(on_glarea_scroll_event),
		"on_glarea_motion_notify_event", G_CALLBACK(on_glarea_motion_notify_event),
		"on_glarea_button_press_event", G_CALLBACK(on_glarea_button_press_event),
		"on_glarea_button_release_event", G_CALLBACK(on_glarea_button_release_event),
		"on_glarea_key_press_event", G_CALLBACK(on_glarea_key_press_event),
		NULL);
}

gboolean on_glarea_key_press_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	(void)widget;
	(void)user_data;
	GdkEventKey *key = &event->key;
	if (key->state == GDK_CONTROL_MASK) {
		switch (key->keyval) {
		case GDK_KEY_minus:
			camera_zoom(&e_camera, -0.2f);
			break;
		case GDK_KEY_plus:
		case GDK_KEY_equal:
			camera_zoom(&e_camera, 0.2f);
			break;
		}
	}
	return TRUE;
}

gboolean on_glarea_button_press_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	(void)widget;
	(void)user_data;
	GdkEventButton *button = &event->button;
	if (button->button == 1) {
		cursor_pressed = TRUE;
		cursor_press_xpos = button->x;
		cursor_press_ypos = button->y;
	}
	return TRUE;
}

gboolean on_glarea_button_release_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	(void)widget;
	(void)user_data;
	GdkEventButton *button = &event->button;
	if (button->button == 1) {
		cursor_pressed = FALSE;
		if (fabs(button->x - cursor_press_xpos) < 3 && fabs(button->y - cursor_press_ypos) < 3)
			satellite_select(button->x, button->y);
	}
	return TRUE;
}

gboolean on_glarea_motion_notify_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	(void)widget;
	(void)user_data;
	GdkEventMotion *motion = &event->motion;
	if (cursor_pressed) {
		float d_ang_x = (2.f * (float)G_PI / e_gl_ctx.res_x);
		float d_ang_y = ((float)G_PI / e_gl_ctx.res_y);
		float ang_x = (float)(cursor_xpos - motion->x) * d_ang_x;
		float ang_y = (float)(cursor_ypos - motion->y) * d_ang_y;

		vec3 view_dir;
		glm_vec3_sub(e_camera.target, e_camera.pos, view_dir);
		glm_vec3_normalize(view_dir);
		float cam_ang = acosf(glm_vec3_dot(view_dir, e_camera.up));
		ang_y = glm_clamp(1e-2f, cam_ang + ang_y, (float)G_PI - 1e-2f) - cam_ang;

		vec3 right;
		glm_vec3_cross(e_camera.up, view_dir, right);

		mat4 rot_mat;
		glm_mat4_identity(rot_mat);
		glm_rotate_at(rot_mat, e_camera.target, ang_x, e_camera.up);
		glm_rotate_at(rot_mat, e_camera.target, ang_y, right);
		glm_mat4_mulv3(rot_mat, e_camera.pos, 1.f, e_camera.pos);

		camera_view_update(&e_camera);
	}
	cursor_xpos = motion->x;
	cursor_ypos = motion->y;
	return TRUE;
}

gboolean on_glarea_scroll_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	(void)widget;
	(void)user_data;
	GdkEventScroll *scroll = &event->scroll;

	camera_zoom(&e_camera, scroll->delta_y * 0.2 * (gs_invert_scroll ? 1 : -1));

	return TRUE;
}
