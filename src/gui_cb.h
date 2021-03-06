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

#ifndef GUI_CB_H
#define GUI_CB_H

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gstdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "extract.h"
#include "common_archive.h"
#include "bzip2.h"
#include "gzip.h"
#include "tar.h"
#include "rar.h"
#include "zip.h"
#include "add.h"
#include "iso.h"

unsigned long long int file_size, file_offset;
short int response;
gchar *name;
gchar *permissions;
GtkWidget *dialog , *textview, *scrollwin, *vbox, *OutputWindow , *view_window, *archive_properties_win;
Extract_dialog_data *extract_window;
GtkTextBuffer *textbuf , *viewtextbuf;
GtkTextIter enditer , start, end;
gchar *ComboArchiveType;
Add_dialog_data *add_window;

gboolean am_a_report_child_stderr (GIOChannel *ioc, GIOCondition cond, gpointer data);

void am_a_deselect_all (GtkMenuItem *menuitem , gpointer user_data);
void am_a_select_all (GtkMenuItem *menuitem , gpointer user_data);

void am_a_new_archive (GtkMenuItem *menuitem, gpointer user_data);
void am_a_open_archive (GtkMenuItem *menuitem, gpointer data);
void am_a_close_archive (GtkMenuItem *menuitem, gpointer user_data);
void am_a_delete_archive (GtkMenuItem *menuitem, gpointer user_data);
void am_a_add_files_archive (GtkMenuItem *menuitem, gpointer data );
void am_a_extract_archive (GtkMenuItem *menuitem , gpointer user_data );

gchar *am_a_open_file_dialog();

void am_a_close_archive (GtkMenuItem *menuitem, gpointer user_data);
void am_a_cancel_archive ( GtkMenuItem *menuitem , gpointer data );
void gui_cb_application_quit(GtkMenuItem *menuitem, gpointer user_data);

void am_a_show_cmd_line_output( GtkMenuItem *menuitem );
void am_a_create_liststore ( unsigned short int nc, gchar *columns_names[] , GType columns_types[], AM_Archive *archive);

int ShowGtkMessageDialog ( GtkWindow *window, int mode, int type, int button, const gchar *message1, const gchar *message2);

int am_a_detect_archive_type ( AM_Archive *archive , gchar *filename );
gboolean isTar ( FILE *ptr );

gboolean treeview_select_search (GtkTreeModel *model,gint column,const gchar *key,GtkTreeIter *iter,gpointer search_data);

void ConcatenateFileNames (GtkTreeModel *model, GtkTreePath *treepath, GtkTreeIter *iter, GString *data);
void ConcatenateFileNames2 (gchar *filename , GString *data);
void EmptyTextBuffer ();
void am_a_cat_filenames (GtkTreeModel *model, GtkTreePath *treepath, GtkTreeIter *iter, GString *data);
void am_a_cat_filenames_basename (GtkTreeModel *model, GtkTreePath *treepath, GtkTreeIter *iter, GString *data);

void am_a_watch_child ( GPid pid, gint status, gpointer data);
gboolean am_a_run_command ( gchar *command , gboolean watch_child_flag );
void am_a_about (GtkMenuItem *menuitem, gpointer user_data);

void addFileM_cb ();
void addFolderM_cb ();
void extractEverythingM_cb ();
void extractSelectedM_cb ();

void indicateProcessingData (gboolean processing);

#endif /* GUI_CB_H */
