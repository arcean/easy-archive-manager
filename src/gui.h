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

#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "common_archive.h"
#include "config.h"

#ifdef GNM_USE_HILDON
#include <hildon/hildon.h>
#endif

GtkWidget *MainWindow;
GtkWidget *vbox1;
GtkWidget *vbox_body;
GtkNotebook *notebook;
GtkWidget *hbox_sb;
GtkWidget *menubar1;
GtkWidget *menuitem1;
GtkWidget *menuitem1_menu;
GtkWidget *new1;
GtkWidget *open1;
GtkWidget *viewport1,*viewport2;
GtkWidget *ebox;
GtkWidget *pad_image;

GtkWidget *separatormenuitem1;
GtkWidget *separatormenuitem2;
GtkWidget *separatormenuitem3;
GtkWidget *separatormenuitem4;
GtkWidget *tmp_dir;
GtkWidget *quit1;
GtkWidget *close1;
GtkWidget *menuitem2;
GtkWidget *menuitem2_menu;
GtkWidget *addfile;
GtkWidget *extract_menu;
GtkWidget *delete_menu;
GtkWidget *image1;
GtkWidget *image2;
GtkWidget *menuitem4;
GtkWidget *select_all;
GtkWidget *deselect_all;
GtkWidget *menuitem4_menu;
GtkWidget *about1;
GtkWidget *view_shell_output1;
GtkWidget *toolbar1;
GtkIconSize tmp_toolbar_icon_size;
GtkWidget *tmp_image;
GtkWidget *New_button;
GtkWidget *Open_button;
GtkWidget *Stop_button;
GtkWidget *separatortoolitem1;
GtkWidget *separatortoolitem2;
GtkWidget *separatortoolitem3;
GtkWidget *AddFile_button;
GtkWidget *Extract_button;
GtkWidget *Delete_button;
GtkAccelGroup *accel_group;
GtkWidget *addMenu;
GtkWidget *addFileM;
GtkWidget *addFolderM;
GtkWidget *extractMenu;
GtkWidget *extractEverythingM;
GtkWidget *extractSelectedM;
#ifdef GNM_USE_HILDON
HildonButton *newB;
HildonButton *openB;
HildonButton *closeArchiveB;
HildonButton *quitB;
HildonButton *settingsB;
HildonButton *aboutB;
HildonButton *shellOutputB;
#endif

void am_a_add_page (AM_Archive *archive);
GtkWidget *gui_create_main_window(void);
gint am_a_find_archive_index (gint page_num);
gint am_a_get_new_archive_idx();
void set_label (GtkWidget *label , gchar *text);

#endif /* GUI_H */
