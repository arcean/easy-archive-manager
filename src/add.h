/*
 *  Copyright (c) 2010,2011 Tomasz Pieniążek <t.pieniazek@gazeta.pl>
 *  Copyright (c) 2005,2006 Giuseppe Torelli - <colossus73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#ifndef ADD_H
#define ADD_H

#include "common_archive.h"

typedef struct
{
	GtkWidget *dialog1;
	GtkWidget *dialog_vbox1;
	GtkWidget *vbox1;
	GtkWidget *vbox6;
	GtkWidget *vbox7;
	GtkWidget *vbox8;
	GtkWidget *hbox1;
	GtkWidget *hbox2;
	GtkWidget *hbox3;
	GtkWidget *frame5;
	GtkWidget *frame4;
	GtkWidget *remove_button;
	GtkWidget *add_files_button;
	GtkWidget *label2;
	GtkWidget *label3;
	GtkWidget *label4;
	GtkWidget *hbuttonbox2;
	GtkWidget *alignment4;
	GtkWidget *alignment5;
	GtkWidget *alignment6;
	GtkWidget *scrolledwindow3;
	GtkWidget *file_list_treeview;
	GtkListStore *file_liststore;
	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;
	GtkTreeViewColumn *column;
	GtkWidget *add_image;
	GtkWidget *add_hbox;
	GtkWidget *add_label;
	GtkWidget *cancel_button;
	GtkWidget *add_button;
	GtkWidget *dialog_action_area2;
} Add_dialog_data;

Add_dialog_data *am_a_create_add_dialog (AM_Archive *archive);
gchar *am_a_parse_add_dialog_options ( AM_Archive *archive, Add_dialog_data *dialog_data );
void activate_remove_button (GtkTreeModel *tree_model, GtkTreePath *path, GtkTreeIter *iter, Add_dialog_data *data);
gchar* am_a_select_files_to_add (AM_Archive *archive);
gchar *am_a_add_single_files ( AM_Archive *archive , GString *names);
void add_files_liststore (gchar *file_path, GtkListStore *liststore);
void remove_files_liststore (GtkWidget *widget, gpointer data);
void remove_foreach_func (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, GList **rowref_list);
void fix_adjustment_value (GtkAdjustment *adjustment, gpointer user_data);
AM_Archive *am_a_new_archive_dialog (gchar *path);

#endif
