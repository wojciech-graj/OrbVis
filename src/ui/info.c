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

#include "info.h"

#include "camera.h"
#include "phys.h"
#include "type.h"

#include "TLE.h"
#include "satcat_code.h"
#include <cglm/mat3.h>
#include <cglm/mat4.h>
#include <cglm/util.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>

#include <math.h>

enum InfoItem {
	INFO_NAME = 0u,
	INFO_CATNUM,
	INFO_ID,
	INFO_PAYLOAD,
	INFO_STATUS,
	INFO_LAUNCH_SITE,
	INFO_LAUNCH_DATE,
	INFO_SOURCE,
	INFO_DECAY_DATE,
	INFO_ORBITAL_STATUS,
	INFO_PERIOD,
	INFO_INCLINATION,
	INFO_APOGEE,
	INFO_PERIGEE,
	INFO_RADAR_CS,
	INFO_POSITION_TEME,
	INFO_POSITION_ECEF,
	INFO_POSITION_LLH,
	INFO_VELOCITY,
	NUM_INFO_ITEMS,
};

static const char *info_item_ids[] = {
	[INFO_NAME] = "info_name",
	[INFO_CATNUM] = "info_catnum",
	[INFO_ID] = "info_id",
	[INFO_PAYLOAD] = "info_payload",
	[INFO_STATUS] = "info_status",
	[INFO_LAUNCH_SITE] = "info_launch_site",
	[INFO_LAUNCH_DATE] = "info_launch_date",
	[INFO_SOURCE] = "info_source",
	[INFO_DECAY_DATE] = "info_decay_date",
	[INFO_ORBITAL_STATUS] = "info_status_code",
	[INFO_PERIOD] = "info_period",
	[INFO_INCLINATION] = "info_inclination",
	[INFO_APOGEE] = "info_apogee",
	[INFO_PERIGEE] = "info_perigee",
	[INFO_RADAR_CS] = "info_radar_cs",
	[INFO_POSITION_TEME] = "info_position_teme",
	[INFO_POSITION_ECEF] = "info_position_ecef",
	[INFO_POSITION_LLH] = "info_position_llh",
	[INFO_VELOCITY] = "info_velocity",
};

static GtkButton *jump_to_button;

static void on_info_jump_to_clicked(GtkButton *button, gpointer user_data);
static void fmt_scdate(gchar *buf, gulong n, struct SCDate *date);
static void fmt_coords(gchar *buf, gulong n, double x, double y, double z);

static GtkLabel *info_items[NUM_INFO_ITEMS];
static struct Satellite *satellite = NULL;

void info_init(GtkBuilder *builder)
{
	unsigned i;
	for (i = 0; i < NUM_INFO_ITEMS; i++)
		info_items[i] = GTK_LABEL(gtk_builder_get_object(builder, info_item_ids[i]));
	gtk_builder_add_callback_symbols(builder,
		"on_info_jump_to_clicked", G_CALLBACK(on_info_jump_to_clicked),
		NULL);
	jump_to_button = GTK_BUTTON(gtk_builder_get_object(builder, "info_jump_to"));
}

void on_info_jump_to_clicked(GtkButton *button, gpointer user_data)
{
	(void)button;
	(void)user_data;

	double r[3], v[3];
	getRVForDate(&satellite->tle, e_phys.epoch_ms, r, v);

	vec3 pos;
	glm_mat4_mulv3(e_phys.teme_to_world, (vec3){ r[0], r[1], r[2] }, 1.f, pos);
	float rad = glm_vec3_norm(pos);
	e_camera.rad = glm_clamp(rad, 1.1f, 10.f);
	glm_vec3_scale(pos, e_camera.rad / rad, e_camera.pos);
	camera_view_update(&e_camera);
}

void fmt_scdate(gchar *buf, gulong n, struct SCDate *date)
{
	g_snprintf(buf, n, "%04u-%02u-%02u", date->year, date->month, date->day);
}

void info_show(struct Satellite *sat)
{
	gchar buf[11];
	satellite = sat;
	gtk_label_set_text(info_items[INFO_NAME], satellite->name);
	g_snprintf(buf, 11, "%05u", satellite->satcat.catnum);
	gtk_label_set_text(info_items[INFO_CATNUM], buf);
	gtk_label_set_text(info_items[INFO_ID], satellite->satcat.id);
	gtk_label_set_text(info_items[INFO_PAYLOAD], ((satellite->satcat.payload) ? "YES" : "NO"));
	gtk_label_set_text(info_items[INFO_STATUS], sc_status_str(satellite->satcat.opstat));
	gtk_label_set_text(info_items[INFO_LAUNCH_SITE], sc_launch_site_str(SC_STR5_TO_CODE(satellite->satcat.launch_site)));
	fmt_scdate(buf, 11, &satellite->satcat.launch_date);
	gtk_label_set_text(info_items[INFO_LAUNCH_DATE], buf);
	gtk_label_set_text(info_items[INFO_SOURCE], sc_source_str(SC_STR5_TO_CODE(satellite->satcat.source)));
	fmt_scdate(buf, 11, &satellite->satcat.decay_date);
	gtk_label_set_text(info_items[INFO_DECAY_DATE], buf);
	memcpy(buf, satellite->satcat.status_code, 3);
	buf[3] = '\0';
	gtk_label_set_text(info_items[INFO_ORBITAL_STATUS], buf);
	g_snprintf(buf, 11, "%07.1f", satellite->satcat.period);
	gtk_label_set_text(info_items[INFO_PERIOD], buf);
	g_snprintf(buf, 11, "%05.1f", satellite->satcat.inc_deg);
	gtk_label_set_text(info_items[INFO_INCLINATION], buf);
	g_snprintf(buf, 11, "%06u", satellite->satcat.apogee);
	gtk_label_set_text(info_items[INFO_APOGEE], buf);
	g_snprintf(buf, 11, "%06u", satellite->satcat.perigee);
	gtk_label_set_text(info_items[INFO_PERIGEE], buf);
	g_snprintf(buf, 11, "%08.4f", satellite->satcat.radar_cs);
	gtk_label_set_text(info_items[INFO_RADAR_CS], buf);

	gtk_widget_set_sensitive(GTK_WIDGET(jump_to_button), TRUE);
}

void info_hide(void)
{
	satellite = NULL;
	unsigned i;
	for (i = 0; i < NUM_INFO_ITEMS; i++)
		gtk_label_set_text(info_items[i], "");

	gtk_widget_set_sensitive(GTK_WIDGET(jump_to_button), FALSE);
}

void fmt_coords(gchar *buf, gulong n, double x, double y, double z)
{
	g_snprintf(buf, n, "X=%011.3f\nY=%011.3f\nZ=%011.3f", x, y, z);
}

void info_tic(void)
{
	if (satellite) {
		gchar buf[48];
		double r[3], v[3];
		getRVForDate(&satellite->tle, e_phys.epoch_ms, r, v);

		double vel = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		g_snprintf(buf, 48, "%08.5f", vel);

		gtk_label_set_text(info_items[INFO_VELOCITY], buf);
		fmt_coords(buf, 48, r[0], r[1], r[2]);
		gtk_label_set_text(info_items[INFO_POSITION_TEME], buf);

		mat3 t; /* TEME to ECEF */
		glm_vec3_copy((vec3){ 0.f, 0.f, 1.f }, t[2]);
		glm_vec3_copy((vec3){ (float)cos(e_phys.gmst), (float)-sin(e_phys.gmst), 0.f }, t[0]);
		glm_vec3_cross(t[2], t[0], t[1]);
		glm_vec3_norm(t[1]);

		vec3 rf, recef;
		glm_vec3_copy((vec3){ r[0], r[1], r[2] }, rf);
		glm_mat3_mulv(t, rf, recef);
		fmt_coords(buf, 48, recef[0], recef[1], recef[2]);
		gtk_label_set_text(info_items[INFO_POSITION_ECEF], buf);

		float lgt = atan2f(recef[1], recef[0]) * (180.f / (float)G_PI);
		float algt = fabsf(lgt);
		float lat = atan2f(recef[2], glm_vec2_norm(recef)) * (180.f / (float)G_PI);
		float alat = fabsf(lat);
		float tmp;
		g_snprintf(buf, 48, "%02u\u00B0%02u'%c\n%03u\u00B0%02u'%c\n%011.3f",
			(unsigned)alat, (unsigned)(modff(alat, &tmp) * 60.f), ((signbit(lat)) ? 'S' : 'N'),
			(unsigned)algt, (unsigned)(modff(algt, &tmp) * 60.f), ((signbit(lgt)) ? 'W' : 'E'),
			(double)glm_vec3_norm(recef));
		gtk_label_set_text(info_items[INFO_POSITION_LLH], buf);
	}
}
