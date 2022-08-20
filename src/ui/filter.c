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

#include "filter.h"

#include "catalog.h"
#include "satcat_code_list.h"

#include <float.h>

#define FILTER_FUNC_BETWEEN(field, target, var) ((field_active[field]) && (((target)[0] >= (var)) || ((target)[1] <= (var))))
#define SC_DATE_LESS(d1, d2) (((d1).year < (d2).year) ? (TRUE) : (((d1).year == (d2).year) ? (((d1).month < (d2).month) ? (TRUE) : (((d1).month == (d2).month) ? ((d1).day < (d2).day) : (FALSE))) : (FALSE)))

enum FilterCombo {
	COMBO_STATUS = 0u,
	COMBO_LAUNCH_SITE,
	COMBO_SOURCE,
	NUM_FILTER_COMBO,
};

enum FilterEntry {
	ENTRY_PERIGEE = 0u,
	ENTRY_APOGEE,
	ENTRY_INCLINATION,
	ENTRY_PERIOD,
	ENTRY_LAUNCH_DATE,
	NUM_FILTER_ENTRY,
};

enum FilterField {
	FIELD_STATUS = 0u,
	FIELD_LAUNCH_SITE,
	FIELD_SOURCE,
	FIELD_PERIGEE,
	FIELD_APOGEE,
	FIELD_INCLINATION,
	FIELD_PERIOD,
	FIELD_LAUNCH_DATE,
	NUM_FILTER_FIELD,
};

static const char *combo_item_ids[NUM_FILTER_COMBO] = {
	[COMBO_STATUS] = "filter_status",
	[COMBO_LAUNCH_SITE] = "filter_launch_site",
	[COMBO_SOURCE] = "filter_source",
};

static const char *entry_item_ids[NUM_FILTER_ENTRY][2] = {
	[ENTRY_PERIGEE] = { "filter_low_perigee", "filter_high_perigee" },
	[ENTRY_APOGEE] = { "filter_low_apogee", "filter_high_apogee" },
	[ENTRY_INCLINATION] = { "filter_low_inclination", "filter_high_inclination" },
	[ENTRY_PERIOD] = { "filter_low_period", "filter_high_period" },
	[ENTRY_LAUNCH_DATE] = { "filter_low_launch_date", "filter_high_launch_date" },
};

static GtkComboBoxText *combo_items[NUM_FILTER_COMBO];
static GtkEntry *entry_items[NUM_FILTER_ENTRY][2];
static gboolean field_active[NUM_FILTER_FIELD] = { FALSE };

static gboolean active = FALSE;

static char target_status;
static sc_code_t target_source;
static sc_code_t target_launch_site;
static double target_perigee[2];
static double target_apogee[2];
static double target_inclination[2];
static double target_period[2];
static struct SCDate target_launch_date[2];

static void on_filter_clear_clicked(GtkButton *button, gpointer user_data);
static void on_filter_apply_clicked(GtkButton *button, gpointer user_data);
static void on_filter_clear_status_clicked(GtkButton *button, gpointer user_data);
static void on_filter_clear_source_clicked(GtkButton *button, gpointer user_data);
static void on_filter_clear_launch_site_clicked(GtkButton *button, gpointer user_data);
static void on_filter_launch_date_activate(GtkEntry *entry, gpointer user_data);

static void filter_set_from_entry(enum FilterEntry entry, enum FilterField field, double *target, double dflt[2]);

void on_filter_clear_status_clicked(GtkButton *button, gpointer user_data)
{
	(void)button;
	(void)user_data;
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_items[COMBO_STATUS]), -1);
}

void on_filter_clear_source_clicked(GtkButton *button, gpointer user_data)
{
	(void)button;
	(void)user_data;
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_items[COMBO_SOURCE]), -1);
}

void on_filter_clear_launch_site_clicked(GtkButton *button, gpointer user_data)
{
	(void)button;
	(void)user_data;
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_items[COMBO_LAUNCH_SITE]), -1);
}

void on_filter_launch_date_activate(GtkEntry *entry, gpointer user_data)
{
	(void)user_data;

	const gchar *text = gtk_entry_get_text(entry);
	unsigned y, m = 1, d = 1;
	int nitems = sscanf(text, "%4u-%2u-%2u", &y, &m, &d);
	if (!nitems)
		goto FAIL;
	{
		GTimeZone *tz = g_time_zone_new_utc();
		GDateTime *dt = g_date_time_new(tz, y, m, d, 0, 0, 0);
		if (dt) {
			g_date_time_unref(dt);
			g_time_zone_unref(tz);
			return;
		}
		g_time_zone_unref(tz);
	}
FAIL:
	gtk_entry_set_text(entry, "");
}

void on_filter_clear_clicked(GtkButton *button, gpointer user_data)
{
	(void)button;
	(void)user_data;

	unsigned i, j;
	for (i = 0; i < NUM_FILTER_COMBO; i++)
		gtk_combo_box_set_active(GTK_COMBO_BOX(combo_items[i]), -1);
	for (i = 0; i < NUM_FILTER_ENTRY; i++)
		for (j = 0; j < 2; j++)
			gtk_entry_set_text(entry_items[i][j], "");
	for (i = 0; i < NUM_FILTER_FIELD; i++)
		field_active[i] = FALSE;

	active = FALSE;
	gtk_tree_model_filter_refilter(e_catalog_filter);
}

void filter_set_from_entry(enum FilterEntry entry, enum FilterField field, double *target, double dflt[2])
{
	field_active[field] = FALSE;
	unsigned j;
	for (j = 0; j < 2; j++) {
		const gchar *text = gtk_entry_get_text(entry_items[entry][j]);
		gchar *endptr;
		target[j] = g_ascii_strtod(text, &endptr);
		if (endptr == text)
			target[j] = dflt[j];
		else
			field_active[field] = TRUE;
	}
}

void on_filter_apply_clicked(GtkButton *button, gpointer user_data)
{
	(void)button;
	(void)user_data;

	int idx;

	idx = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_items[COMBO_STATUS]));
	field_active[FIELD_STATUS] = (idx != -1);
	if (field_active[FIELD_STATUS])
		target_status = SCSTAT[idx];

	idx = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_items[COMBO_SOURCE]));
	field_active[FIELD_SOURCE] = (idx != -1);
	if (field_active[FIELD_SOURCE])
		target_source = SCSRC[idx];

	idx = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_items[COMBO_LAUNCH_SITE]));
	field_active[FIELD_LAUNCH_SITE] = (idx != -1);
	if (field_active[FIELD_LAUNCH_SITE])
		target_launch_site = SCSITE[idx];

	filter_set_from_entry(ENTRY_PERIGEE, FIELD_PERIGEE, target_perigee, (double[2]){ 0, DBL_MAX });
	filter_set_from_entry(ENTRY_APOGEE, FIELD_APOGEE, target_apogee, (double[2]){ 0, DBL_MAX });
	filter_set_from_entry(ENTRY_INCLINATION, FIELD_INCLINATION, target_inclination, (double[2]){ 0, 360.0 });
	filter_set_from_entry(ENTRY_PERIOD, FIELD_PERIOD, target_period, (double[2]){ 0, DBL_MAX });

	field_active[FIELD_LAUNCH_DATE] = FALSE;
	unsigned i;
	for (i = 0; i < 2; i++) {
		target_launch_date[i].year = (unsigned[2]){ 0, 9999 }[i];
		target_launch_date[i].month = 1;
		target_launch_date[i].day = 1;
		int nitems = sscanf(gtk_entry_get_text(entry_items[ENTRY_LAUNCH_DATE][i]), "%4u-%2u-%2u", &target_launch_date[i].year, &target_launch_date[i].month, &target_launch_date[i].day);
		if (nitems)
			field_active[FIELD_LAUNCH_DATE] = TRUE;
	}

	active = TRUE;
	gtk_tree_model_filter_refilter(e_catalog_filter);
}

gboolean filter_func(struct Satellite *satellite)
{
	if (!active)
		return TRUE;

	return !((field_active[FIELD_STATUS] && satellite->satcat.opstat != target_status)
		|| (field_active[FIELD_SOURCE] && SC_STR5_TO_CODE(satellite->satcat.source) != target_source)
		|| (field_active[FIELD_LAUNCH_SITE] && SC_STR5_TO_CODE(satellite->satcat.launch_site) != target_launch_site)
		|| FILTER_FUNC_BETWEEN(FIELD_PERIGEE, target_perigee, satellite->satcat.perigee)
		|| FILTER_FUNC_BETWEEN(FIELD_APOGEE, target_apogee, satellite->satcat.apogee)
		|| FILTER_FUNC_BETWEEN(FIELD_INCLINATION, target_inclination, satellite->satcat.inc_deg)
		|| FILTER_FUNC_BETWEEN(FIELD_PERIOD, target_period, satellite->satcat.period)
		|| (field_active[FIELD_LAUNCH_DATE] && (SC_DATE_LESS(satellite->satcat.launch_date, target_launch_date[0]) || SC_DATE_LESS(target_launch_date[1], satellite->satcat.launch_date))));
}

void filter_init(GtkBuilder *builder)
{
	gtk_builder_add_callback_symbols(builder,
		"on_filter_clear_clicked", G_CALLBACK(on_filter_clear_clicked),
		"on_filter_apply_clicked", G_CALLBACK(on_filter_apply_clicked),
		"on_filter_clear_status_clicked", G_CALLBACK(on_filter_clear_status_clicked),
		"on_filter_clear_source_clicked", G_CALLBACK(on_filter_clear_source_clicked),
		"on_filter_clear_launch_site_clicked", G_CALLBACK(on_filter_clear_launch_site_clicked),
		"on_filter_launch_date_activate", G_CALLBACK(on_filter_launch_date_activate),
		NULL);

	unsigned i, j;
	for (i = 0; i < NUM_FILTER_COMBO; i++)
		combo_items[i] = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, combo_item_ids[i]));
	for (i = 0; i < NUM_FILTER_ENTRY; i++)
		for (j = 0; j < 2; j++)
			entry_items[i][j] = GTK_ENTRY(gtk_builder_get_object(builder, entry_item_ids[i][j]));

	for (i = 0; i < NUM_SCSTAT; i++)
		gtk_combo_box_text_append(combo_items[COMBO_STATUS], NULL, sc_status_str(SCSTAT[i]));
	for (i = 0; i < NUM_SCSRC; i++)
		gtk_combo_box_text_append(combo_items[COMBO_SOURCE], NULL, sc_source_str(SCSRC[i]));
	for (i = 0; i < NUM_SCSITE; i++)
		gtk_combo_box_text_append(combo_items[COMBO_LAUNCH_SITE], NULL, sc_launch_site_str(SCSITE[i]));
}
