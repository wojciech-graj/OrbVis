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

#include "catalog.h"

#include "filter.h"
#include "info.h"
#include "satcat_code.h"

enum Column {
	COL_SELECT = 0u,
	COL_CATNUM,
	COL_NAME,
	COL_ID,
	COL_STATUS,
	COL_SOURCE,
	COL_LAUNCH_DATE,
	COL_PERIOD,
	COL_INCLINATION,
	COL_APOGEE,
	COL_PERIGEE,
	NUM_COLS,
};

enum Page {
	PAGE_CATALOG = 0u,
	PAGE_INFO,
	PAGE_FILTER,
};

static const char *column_ids[] = {
	[COL_SELECT] = "col_select",
	[COL_CATNUM] = "col_catnum",
	[COL_NAME] = "col_name",
	[COL_ID] = "col_id",
	[COL_STATUS] = "col_status",
	[COL_SOURCE] = "col_source",
	[COL_LAUNCH_DATE] = "col_launch_date",
	[COL_PERIOD] = "col_period",
	[COL_INCLINATION] = "col_inclination",
	[COL_APOGEE] = "col_apogee",
	[COL_PERIGEE] = "col_perigee",
};

GtkTreeModelFilter *e_catalog_filter = NULL;

static GtkTreeViewColumn *columns[NUM_COLS];
static GtkWindow *window_catalog;
static GtkTreeSortable *sort = NULL;
static gchar search_text[64] = { 0 };
static enum Column sort_col = COL_CATNUM;
static GtkSortType sort_type = GTK_SORT_ASCENDING;
static GtkTreeView *satellite_view;
static GtkListStore *satellite_store;
static GtkTreeSelection *selection;
static char *str_satellite_store = NULL;

static const unsigned OFFSET_ID = offsetof(struct Satellite, satcat) + offsetof(struct SatCat, id);
static const unsigned OFFSET_NAME = offsetof(struct Satellite, name);

static gboolean on_window_catalog_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
static void on_catalog_clicked(GtkToolButton *toolbutton, gpointer user_data);
static void on_column_select_cell_toggled(GtkCellRendererToggle *cell_renderer, char *path, gpointer user_data);
static void on_catalog_search_search_changed(GtkSearchEntry *entry, gpointer user_data);
static void on_col_clicked(GtkTreeViewColumn *treeviewcolumn, gpointer user_data);
static void on_catalog_notebook_switch_page(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer user_data);

static gboolean catalog_view_filter_visible_func(GtkTreeModel *model, GtkTreeIter *iter, gpointer data);
static gint catalog_view_search_compare_func(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer data);

static void init_col_with_data_func(GtkCellRenderer *render, GObject *obj, GtkTreeCellDataFunc func, gpointer user_data, int width_chars);
static void init_col_from_store_col(GtkCellRenderer *render, GObject *obj, unsigned col, int width_chars);

static void cell_data_func_select(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data);
static void cell_data_func_str(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data);
static void cell_data_func_status(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data);

static void catalog_construct_views(void);
static void catalog_deconstruct_views(void);

void on_catalog_clicked(GtkToolButton *toolbutton, gpointer user_data)
{
	(void)toolbutton;
	(void)user_data;
	gtk_widget_show_all(GTK_WIDGET(window_catalog));
}

gboolean on_window_catalog_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	(void)widget;
	(void)event;
	(void)user_data;
	gtk_widget_hide(GTK_WIDGET(window_catalog));
	return TRUE;
}

void on_column_select_cell_toggled(GtkCellRendererToggle *cell_renderer, char *path, gpointer user_data)
{
	(void)cell_renderer;
	(void)user_data;
	GtkTreeIter iter;
	gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(e_catalog_filter), &iter, path);
	struct Satellite *satellite;
	gtk_tree_model_get(GTK_TREE_MODEL(e_catalog_filter), &iter,
		0, &satellite,
		-1);
	satellite_select_ptr(satellite);
}

void on_catalog_search_search_changed(GtkSearchEntry *entry, gpointer user_data)
{
	(void)user_data;
	const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
	unsigned i;
	for (i = 0; i < 63 && text[i]; i++)
		search_text[i] = g_ascii_toupper(text[i]);
	search_text[i] = '\0';
	gtk_tree_model_filter_refilter(e_catalog_filter);
}

void on_col_clicked(GtkTreeViewColumn *treeviewcolumn, gpointer user_data)
{
	(void)user_data;
	unsigned i;
	for (i = 0; treeviewcolumn != columns[i]; i++)
		;
	if (sort_col == i) {
		sort_type = (sort_type == GTK_SORT_ASCENDING) ? GTK_SORT_DESCENDING : GTK_SORT_ASCENDING;
	} else {
		sort_type = GTK_SORT_ASCENDING;
		gtk_tree_view_column_set_sort_indicator(columns[sort_col], FALSE);
		gtk_tree_view_column_set_sort_indicator(treeviewcolumn, TRUE);
		sort_col = i;
	}
	gtk_tree_view_column_set_sort_order(treeviewcolumn, sort_type);
	gtk_tree_sortable_set_sort_column_id(sort, 1, sort_type);
}

void on_catalog_notebook_switch_page(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer user_data)
{
	(void)notebook;
	(void)page;
	(void)user_data;
	switch (page_num) {
	case PAGE_CATALOG:
		info_hide();
		break;
	case PAGE_INFO: {
		GtkTreeModel *model;
		GtkTreeIter iter;
		gboolean selected = gtk_tree_selection_get_selected(selection, &model, &iter);
		if (selected) {
			struct Satellite *satellite;
			gtk_tree_model_get(model, &iter,
				0, &satellite,
				-1);
			info_show(satellite);
		}
	} break;
	}
}

void catalog_construct_views(void)
{
	sort = GTK_TREE_SORTABLE(gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(satellite_store)));
	gtk_tree_sortable_set_sort_func(sort, 1, catalog_view_search_compare_func, NULL, NULL);

	e_catalog_filter = GTK_TREE_MODEL_FILTER(gtk_tree_model_filter_new(GTK_TREE_MODEL(sort), NULL));
	gtk_tree_model_filter_set_visible_func(e_catalog_filter, catalog_view_filter_visible_func, NULL, NULL);
	g_object_unref(sort);

	gtk_tree_view_set_model(satellite_view, GTK_TREE_MODEL(e_catalog_filter));
	g_object_unref(e_catalog_filter);
}

void catalog_deconstruct_views(void)
{
	gtk_tree_view_set_model(satellite_view, NULL);
}

void catalog_satellite_changed(struct Satellite *satellite)
{
	GtkTreeIter iter;
	GtkTreeModel *model = GTK_TREE_MODEL(e_catalog_filter);
	if (!gtk_tree_model_get_iter_first(model, &iter))
		return;
	struct Satellite *iter_satellite;
	gtk_tree_model_get(model, &iter, 0, &iter_satellite, -1);
	while (iter_satellite != satellite) {
		if (!gtk_tree_model_iter_next(model, &iter))
			return;
		gtk_tree_model_get(model, &iter, 0, &iter_satellite, -1);
	}
	GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(e_catalog_filter), &iter);
	gtk_tree_model_row_changed(GTK_TREE_MODEL(e_catalog_filter), path, &iter);
	gtk_tree_path_free(path);
}

gboolean catalog_view_filter_visible_func(GtkTreeModel *model, GtkTreeIter *iter, gpointer data)
{
	(void)data;
	struct Satellite *satellite;
	gtk_tree_model_get(model, iter,
		0, &satellite,
		-1);
	return (g_strstr_len(satellite->satcat.name, 24, search_text) != NULL) && (filter_func(satellite));
}

gint catalog_view_search_compare_func(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer data)
{
	(void)data;
	struct Satellite *sa, *sb;
	gtk_tree_model_get(model, a,
		0, &sa,
		-1);
	gtk_tree_model_get(model, b,
		0, &sb,
		-1);
	switch (sort_col) {
	case COL_SELECT:
		return (sb->orbit_idx != UINT32_MAX) - (sa->orbit_idx != UINT32_MAX);
	case COL_CATNUM:
		return ((gint)sa->satcat.catnum) - (gint)sb->satcat.catnum;
	case COL_NAME:
		return strncmp(sa->name, sb->name, 24);
	case COL_ID:
		return strncmp(sa->satcat.id, sb->satcat.id, 11);
	case COL_STATUS:
		return ((gint)sa->satcat.opstat) - (gint)sb->satcat.opstat;
	case COL_SOURCE:
		return strncmp(sa->satcat.source, sb->satcat.source, 5);
	case COL_LAUNCH_DATE: {
		char *la, *lb;
		gtk_tree_model_get(model, a,
			2, &la,
			-1);
		gtk_tree_model_get(model, b,
			2, &lb,
			-1);
		return strncmp(la, lb, 10);
	}
	case COL_PERIOD: {
		double diff = sa->satcat.period - sb->satcat.period;
		return (diff < 0) ? -1 : ((diff) ? 1 : 0);
	}
	case COL_INCLINATION: {
		double diff = sa->satcat.inc_deg - sb->satcat.inc_deg;
		return (diff < 0) ? -1 : ((diff) ? 1 : 0);
	}
	case COL_APOGEE:
		return ((gint)sa->satcat.apogee) - (gint)sb->satcat.apogee;
	case COL_PERIGEE:
		return ((gint)sa->satcat.perigee) - (gint)sb->satcat.perigee;
	case NUM_COLS:
		return 0;
	}
	return 0;
}

void cell_data_func_select(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
	(void)tree_column;
	(void)data;
	(void)cell;
	struct Satellite *satellite;
	gtk_tree_model_get(tree_model, iter, 0, &satellite, -1);
	g_object_set(cell,
		"active", (gboolean)(satellite->orbit_idx != UINT32_MAX),
		NULL);
}

void cell_data_func_str(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
	(void)tree_column;
	(void)cell;
	char *satellite;
	gtk_tree_model_get(tree_model, iter, 0, &satellite, -1);
	g_object_set(cell,
		"text", satellite + *(unsigned *)data,
		NULL);
}

void cell_data_func_status(GtkTreeViewColumn *tree_column, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
	(void)tree_column;
	(void)data;
	(void)cell;
	struct Satellite *satellite;
	gtk_tree_model_get(tree_model, iter, 0, &satellite, -1);
	g_object_set(cell,
		"text", sc_status_str(satellite->satcat.opstat),
		NULL);
}

void init_col_with_data_func(GtkCellRenderer *render, GObject *obj, GtkTreeCellDataFunc func, gpointer user_data, int width_chars)
{
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(obj), render, FALSE);
	gtk_tree_view_column_set_cell_data_func(GTK_TREE_VIEW_COLUMN(obj), render, func, user_data, NULL);
	if (width_chars)
		g_object_set(render, "width-chars", width_chars, NULL);
}

void init_col_from_store_col(GtkCellRenderer *render, GObject *obj, unsigned col, int width_chars)
{
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(obj), render, FALSE);
	gtk_tree_view_column_set_attributes(GTK_TREE_VIEW_COLUMN(obj), render, "text", col, NULL);
	if (width_chars)
		g_object_set(render, "width-chars", width_chars, NULL);
}

void catalog_satellites_fill(struct Satellite *satellites, size_t n_satellites)
{
	catalog_deconstruct_views();
	gtk_list_store_clear(satellite_store);
	g_free(str_satellite_store);
	str_satellite_store = g_malloc(31 * n_satellites);
	char *str = str_satellite_store;
	unsigned i;
	for (i = 0; i < n_satellites; i++) {
		const struct SCDate *date = &satellites[i].satcat.launch_date;
		g_snprintf(str, 31, "%04u-%02u-%02u %07.1f %05.1f %.5s", date->year, date->month, date->day, satellites[i].satcat.period, satellites[i].satcat.inc_deg, satellites[i].satcat.source);
		str[10] = str[18] = str[24] = '\0';
		gtk_list_store_insert_with_values(satellite_store, NULL, -1,
			0, &satellites[i],
			1, satellites[i].satcat.catnum,
			2, str,
			3, str + 11,
			4, str + 19,
			5, str + 25,
			6, satellites[i].satcat.apogee,
			7, satellites[i].satcat.perigee,
			-1);
		str += 31;
	}
	catalog_construct_views();
}

void catalog_init(GtkBuilder *builder)
{
	gtk_builder_add_callback_symbols(builder,
		"on_catalog_clicked", G_CALLBACK(on_catalog_clicked),
		"on_window_catalog_delete_event", G_CALLBACK(on_window_catalog_delete_event),
		"on_catalog_search_search_changed", G_CALLBACK(on_catalog_search_search_changed),
		"on_col_clicked", G_CALLBACK(on_col_clicked),
		"on_catalog_notebook_switch_page", G_CALLBACK(on_catalog_notebook_switch_page),
		NULL);

	window_catalog = GTK_WINDOW(gtk_builder_get_object(builder, "window_catalog"));
	satellite_store = GTK_LIST_STORE(gtk_builder_get_object(builder, "satellite_store"));
	g_object_ref(satellite_store);
	satellite_view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "catalog_view"));
	selection = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "catalog_selection"));

	GtkCellRenderer *render_select = gtk_cell_renderer_toggle_new();
	init_col_with_data_func(render_select, gtk_builder_get_object(builder, column_ids[COL_SELECT]), cell_data_func_select, NULL, 0);
	g_signal_connect(render_select, "toggled", G_CALLBACK(on_column_select_cell_toggled), NULL);

	init_col_from_store_col(gtk_cell_renderer_text_new(), gtk_builder_get_object(builder, column_ids[COL_CATNUM]), 1, 5);
	init_col_with_data_func(gtk_cell_renderer_text_new(), gtk_builder_get_object(builder, column_ids[COL_NAME]), cell_data_func_str, (gpointer)&OFFSET_NAME, 24);
	init_col_with_data_func(gtk_cell_renderer_text_new(), gtk_builder_get_object(builder, column_ids[COL_ID]), cell_data_func_str, (gpointer)&OFFSET_ID, 11);
	init_col_with_data_func(gtk_cell_renderer_text_new(), gtk_builder_get_object(builder, column_ids[COL_STATUS]), cell_data_func_status, NULL, 21);
	init_col_from_store_col(gtk_cell_renderer_text_new(), gtk_builder_get_object(builder, column_ids[COL_SOURCE]), 5, 5);
	init_col_from_store_col(gtk_cell_renderer_text_new(), gtk_builder_get_object(builder, column_ids[COL_LAUNCH_DATE]), 2, 10);
	init_col_from_store_col(gtk_cell_renderer_text_new(), gtk_builder_get_object(builder, column_ids[COL_PERIOD]), 3, 7);
	init_col_from_store_col(gtk_cell_renderer_text_new(), gtk_builder_get_object(builder, column_ids[COL_INCLINATION]), 4, 5);
	init_col_from_store_col(gtk_cell_renderer_text_new(), gtk_builder_get_object(builder, column_ids[COL_APOGEE]), 6, 6);
	init_col_from_store_col(gtk_cell_renderer_text_new(), gtk_builder_get_object(builder, column_ids[COL_PERIGEE]), 7, 6);

	unsigned i;
	for (i = 0; i < NUM_COLS; i++)
		columns[i] = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder, column_ids[i]));
}

void catalog_deinit(void)
{
	g_free(str_satellite_store);
}
