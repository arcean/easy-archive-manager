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

#ifndef EXTRACT_H
#define EXTRACT_H

#include "common_archive.h"

typedef struct
{
	GtkWidget *dialog1;
	GtkWidget *dialog_vbox1;
	GtkWidget *vbox1;
	GtkWidget *vbox2;
	GtkWidget *hbox3;
	GtkWidget *extract_to_label;
	GtkWidget *destination_path_entry;
	GtkWidget *button1;
	GtkWidget *image1;
	GtkWidget *hbox4;
	GtkWidget *frame1;
	GtkWidget *frame2;
	GtkWidget *alignment1;
	GtkWidget *alignment2;
	GtkWidget *alignment3;
	GtkWidget *vbox3;
	GtkWidget *vbox4;
	GtkWidget *hbox5;
	GtkWidget *hbox6;
	GtkWidget *cancel_button;
	GtkWidget *extract_label;
	GtkWidget *extract_button;
	GtkWidget *extract_hbox;
} Extract_dialog_data;

Extract_dialog_data *am_a_create_extract_dialog (gint selected ,AM_Archive *archive);
gchar *am_a_parse_extract_dialog_options ( AM_Archive *archive , Extract_dialog_data *dialog_data, GtkTreeSelection *selection);
gboolean am_a_create_temp_directory (gchar tmp_dir[]);
gboolean am_a_delete_temp_directory ( gchar *dir_name, gboolean flag);
void am_a_choose_extraction_directory (GtkWidget *widget, gpointer data);
gchar *am_a_extract_single_files ( AM_Archive *archive , GString *files, gchar *path);

#endif
