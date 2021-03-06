/**
 * @file
 * @author  Tomasz Pieniążek (2010, 2011) <t.pieniazek@gazeta.pl>
 * 	    Giuseppe Torelli (2005, 2006) <colossus73@gmail.com>
 *
 * @section LICENSE
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
 * @section DESCRIPTION
 *
 * Contains GUI for MainWindow and all other needed functions.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>

#include "config.h"

#include "gui.h"
#include "gui_cb.h"
#include "settings.h"

/**
 * Creates a new MainWindow
 *
 */
GtkWidget *gui_create_main_window(void)
{
	GdkPixbuf *icon;
	GtkIconTheme *icon_theme;

	accel_group = gtk_accel_group_new ();

#ifdef GNM_USE_HILDON
	MainWindow = hildon_stackable_window_new ();
#else
	MainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
#endif

	am_a_set_window_title (MainWindow , NULL);

	icon_theme = gtk_icon_theme_get_default();
	icon = gtk_icon_theme_load_icon(icon_theme, "AManager", 24, 0, NULL);
	gtk_window_set_icon (GTK_WINDOW(MainWindow),icon);
	g_signal_connect (G_OBJECT (MainWindow), "delete-event", G_CALLBACK (gui_cb_application_quit), NULL);

	/* Create the menus */
	vbox1 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox1);
	gtk_container_add (GTK_CONTAINER (MainWindow), vbox1);

#ifndef GNM_USE_HILDON
	menubar1 = gtk_menu_bar_new ();
	gtk_widget_show (menubar1);
	gtk_box_pack_start (GTK_BOX (vbox1), menubar1, FALSE, FALSE, 0);

	menuitem1 = gtk_menu_item_new_with_mnemonic ("_File");
	gtk_widget_show (menuitem1);
	gtk_container_add (GTK_CONTAINER (menubar1), menuitem1);

	menuitem1_menu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem1), menuitem1_menu);

	new1 = gtk_image_menu_item_new_from_stock ("gtk-new", accel_group);
	gtk_widget_show (new1);
	gtk_container_add (GTK_CONTAINER (menuitem1_menu), new1);

	open1 = gtk_image_menu_item_new_from_stock ("gtk-open", accel_group);
	gtk_widget_show (open1);
	gtk_container_add (GTK_CONTAINER (menuitem1_menu), open1);

	separatormenuitem1 = gtk_separator_menu_item_new ();
	gtk_widget_show (separatormenuitem1);
	gtk_container_add (GTK_CONTAINER (menuitem1_menu), separatormenuitem1);
	gtk_widget_set_sensitive (separatormenuitem1, FALSE);

  	close1 = gtk_image_menu_item_new_from_stock ("gtk-close", accel_group);
	gtk_widget_set_sensitive (close1,FALSE);
   	gtk_widget_show (close1);
  	gtk_container_add (GTK_CONTAINER (menuitem1_menu), close1);

  	quit1 = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
  	gtk_widget_show (quit1);
  	gtk_container_add (GTK_CONTAINER (menuitem1_menu), quit1);

  	menuitem2 = gtk_menu_item_new_with_mnemonic ("_Action");
  	gtk_widget_show (menuitem2);
  	gtk_container_add (GTK_CONTAINER (menubar1), menuitem2);

  	menuitem2_menu = gtk_menu_new ();
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem2), menuitem2_menu);

  	addfile = gtk_image_menu_item_new_with_mnemonic ("_Add");
  	gtk_widget_set_sensitive (addfile,FALSE);
  	gtk_widget_show (addfile);
  	gtk_container_add (GTK_CONTAINER (menuitem2_menu), addfile);
  	gtk_widget_add_accelerator (addfile, "activate",accel_group,GDK_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  	extract_menu = gtk_image_menu_item_new_with_mnemonic ("_Extract");
  	gtk_widget_show (extract_menu);
	gtk_widget_set_sensitive (extract_menu,FALSE);
  	gtk_container_add (GTK_CONTAINER (menuitem2_menu), extract_menu);
  	gtk_widget_add_accelerator (extract_menu, "activate",accel_group,GDK_e, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  	separatormenuitem2 = gtk_separator_menu_item_new ();
  	gtk_widget_show (separatormenuitem2);
  	gtk_container_add (GTK_CONTAINER (menuitem2_menu), separatormenuitem2);
  	gtk_widget_set_sensitive (separatormenuitem2, FALSE);

  	delete_menu = gtk_image_menu_item_new_from_stock ("gtk-delete", accel_group);
  	gtk_widget_set_sensitive (delete_menu,FALSE);
  	gtk_widget_show (delete_menu);
  	gtk_container_add (GTK_CONTAINER (menuitem2_menu), delete_menu);
  	gtk_widget_add_accelerator (delete_menu, "activate",accel_group,GDK_d, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  	view_shell_output1 = gtk_image_menu_item_new_with_mnemonic ("Command line output");
  	gtk_widget_show (view_shell_output1);
  	gtk_container_add (GTK_CONTAINER (menuitem2_menu), view_shell_output1);
  	gtk_widget_add_accelerator (view_shell_output1, "activate",accel_group,GDK_m, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  	image2 = gtk_image_new_from_stock ("gtk-find-and-replace", GTK_ICON_SIZE_MENU);
  	gtk_widget_show (image2);
  	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (view_shell_output1), image2);

  	separatormenuitem3 = gtk_separator_menu_item_new ();
  	gtk_widget_show (separatormenuitem3);
  	gtk_container_add (GTK_CONTAINER (menuitem2_menu), separatormenuitem3);
  	gtk_widget_set_sensitive (separatormenuitem3, FALSE);

  	select_all = gtk_image_menu_item_new_with_mnemonic ("Select _All");
  	gtk_widget_show (select_all);
  	gtk_container_add (GTK_CONTAINER (menuitem2_menu), select_all);
  	gtk_widget_set_sensitive (select_all, FALSE);
 	gtk_widget_add_accelerator (select_all, "activate",accel_group,GDK_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  	deselect_all = gtk_image_menu_item_new_with_mnemonic ("Dese_lect All");
  	gtk_widget_show (deselect_all);
  	gtk_container_add (GTK_CONTAINER (menuitem2_menu), deselect_all);
  	gtk_widget_set_sensitive (deselect_all, FALSE);
  	gtk_widget_add_accelerator (deselect_all, "activate",accel_group,GDK_l, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  	separatormenuitem4 = gtk_separator_menu_item_new ();
  	gtk_widget_show (separatormenuitem4);
  	gtk_container_add (GTK_CONTAINER (menuitem2_menu), separatormenuitem4);
  	gtk_widget_set_sensitive (separatormenuitem4, FALSE);

  	tmp_dir = gtk_image_menu_item_new_with_mnemonic ("_Settings");
  	gtk_widget_show (tmp_dir);
  	gtk_widget_set_sensitive ( tmp_dir , TRUE);
  	gtk_container_add (GTK_CONTAINER (menuitem2_menu), tmp_dir);
  	gtk_widget_add_accelerator (tmp_dir, "activate",accel_group,GDK_p, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  	tmp_image = gtk_image_new_from_stock ("gtk-properties", GTK_ICON_SIZE_MENU);
  	gtk_widget_show (tmp_image);
  	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tmp_dir), tmp_image);

  	menuitem4 = gtk_menu_item_new_with_mnemonic ("_Help");
  	gtk_widget_show (menuitem4);
  	gtk_container_add (GTK_CONTAINER (menubar1), menuitem4);

  	menuitem4_menu = gtk_menu_new ();
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem4), menuitem4_menu);

	about1 = gtk_image_menu_item_new_from_stock ("gtk-about", accel_group);
	gtk_widget_show (about1);
	gtk_container_add (GTK_CONTAINER (menuitem4_menu), about1);
#else
	HildonSizeType size = HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH;
	HildonAppMenu *menubar1 = HILDON_APP_MENU(hildon_app_menu_new());

	newB = HILDON_BUTTON(hildon_button_new((size), HILDON_BUTTON_ARRANGEMENT_HORIZONTAL));
	hildon_button_set_title(newB, "New archive");
	g_signal_connect_after (newB, "clicked", G_CALLBACK (am_a_new_archive), NULL);
	hildon_app_menu_append(menubar1, GTK_BUTTON(newB));

	openB = HILDON_BUTTON(hildon_button_new((size), HILDON_BUTTON_ARRANGEMENT_HORIZONTAL));
	hildon_button_set_title(openB, "Open archive");
	g_signal_connect_after (openB, "clicked", G_CALLBACK (am_a_open_archive), NULL);
	hildon_app_menu_append(menubar1, GTK_BUTTON(openB));

	closeArchiveB = HILDON_BUTTON(hildon_button_new((size), HILDON_BUTTON_ARRANGEMENT_HORIZONTAL));
	hildon_button_set_title(closeArchiveB, "Close");
	g_signal_connect_after (closeArchiveB, "clicked", G_CALLBACK (am_a_close_archive), NULL);
	hildon_app_menu_append(menubar1, GTK_BUTTON(closeArchiveB));
/*
	quitB = HILDON_BUTTON(hildon_button_new((size), HILDON_BUTTON_ARRANGEMENT_HORIZONTAL));
	hildon_button_set_title(quitB, "Quit");
	g_signal_connect_after (quitB, "clicked", G_CALLBACK (gui_cb_application_quit), NULL);
	hildon_app_menu_append(menubar1, GTK_BUTTON(quitB));
*/
	settingsB = HILDON_BUTTON(hildon_button_new((size), HILDON_BUTTON_ARRANGEMENT_HORIZONTAL));
	hildon_button_set_title(settingsB, "Settings");
	g_signal_connect_after (settingsB, "clicked", G_CALLBACK (am_a_create_settings_window), NULL);
	hildon_app_menu_append(menubar1, GTK_BUTTON(settingsB));

	aboutB = HILDON_BUTTON(hildon_button_new((size), HILDON_BUTTON_ARRANGEMENT_HORIZONTAL));
	hildon_button_set_title(aboutB, "About");
	g_signal_connect_after (aboutB, "clicked", G_CALLBACK (am_a_about), NULL);
	hildon_app_menu_append(menubar1, GTK_BUTTON(aboutB));

//Should not be available in public releases:
	shellOutputB = HILDON_BUTTON(hildon_button_new((size), HILDON_BUTTON_ARRANGEMENT_HORIZONTAL));
	hildon_button_set_title(shellOutputB, "Log");
	g_signal_connect_after (shellOutputB, "clicked", G_CALLBACK (am_a_show_cmd_line_output), NULL);
	//hildon_app_menu_append(menubar1, GTK_BUTTON(shellOutputB));
//*******************************************

	gtk_widget_show_all(GTK_WIDGET(menubar1));
	hildon_window_set_app_menu(HILDON_WINDOW (MainWindow), menubar1);


#endif


	/* Create the toolbar */
	toolbar1 = gtk_toolbar_new ();
	gtk_widget_show (toolbar1);
#ifndef GNM_USE_HILDON
	gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, FALSE, 0);
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH);
#else
	gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_ICONS);
#endif

	tmp_toolbar_icon_size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar1));

#ifndef GNM_USE_HILDON
	tmp_image = gtk_image_new_from_stock ("gtk-new", tmp_toolbar_icon_size);
	gtk_widget_show (tmp_image);
	New_button = (GtkWidget*) gtk_tool_button_new (tmp_image, "New");
	gtk_widget_show (New_button);
	gtk_tool_item_set_homogeneous (GTK_TOOL_ITEM (New_button), FALSE);
	gtk_container_add (GTK_CONTAINER (toolbar1), New_button);

	tmp_image = gtk_image_new_from_stock ("gtk-open", tmp_toolbar_icon_size);
	gtk_widget_show (tmp_image);
	Open_button = (GtkWidget*) gtk_tool_button_new (tmp_image, "Open");
	gtk_widget_show (Open_button);
	gtk_tool_item_set_homogeneous (GTK_TOOL_ITEM (Open_button), FALSE);
	gtk_container_add (GTK_CONTAINER (toolbar1), Open_button);

	separatortoolitem1 = (GtkWidget*) gtk_separator_tool_item_new ();
	gtk_widget_show (separatortoolitem1);
	gtk_container_add (GTK_CONTAINER (toolbar1), separatortoolitem1);
#endif

	tmp_image = gtk_image_new_from_stock ("gtk-add", tmp_toolbar_icon_size);
	gtk_widget_show (tmp_image);
	AddFile_button = (GtkWidget*) gtk_tool_button_new (tmp_image, "Add");
	gtk_widget_set_sensitive (AddFile_button,FALSE);
	gtk_widget_show (AddFile_button);
	gtk_tool_item_set_homogeneous (GTK_TOOL_ITEM (AddFile_button), FALSE);
	gtk_container_add (GTK_CONTAINER (toolbar1), AddFile_button);

	tmp_image = gtk_image_new_from_file("/usr/share/icons/hicolor/48x48/hildon/general_toolbar_folder.png");
	gtk_widget_show (tmp_image);
	Extract_button = (GtkWidget*) gtk_tool_button_new (tmp_image, "Extract");
	gtk_widget_set_sensitive (Extract_button,FALSE);
	gtk_widget_show (Extract_button);
	gtk_tool_item_set_homogeneous (GTK_TOOL_ITEM (Extract_button), FALSE);
	gtk_container_add (GTK_CONTAINER (toolbar1), Extract_button);

	tmp_image = gtk_image_new_from_stock ("gtk-delete", tmp_toolbar_icon_size);
	gtk_widget_show (tmp_image);
	Delete_button = (GtkWidget*) gtk_tool_button_new (tmp_image, "Delete");
	gtk_widget_show (Delete_button);
	gtk_widget_set_sensitive (Delete_button,FALSE);
	gtk_tool_item_set_homogeneous (GTK_TOOL_ITEM (Delete_button), FALSE);
	gtk_container_add (GTK_CONTAINER (toolbar1), Delete_button);

#ifndef GNM_USE_HILDON
	separatortoolitem3 = (GtkWidget*) gtk_separator_tool_item_new ();
	gtk_widget_show (separatortoolitem3);
	gtk_container_add (GTK_CONTAINER (toolbar1), separatortoolitem3);
#endif

	tmp_image = gtk_image_new_from_stock ("gtk-stop", tmp_toolbar_icon_size);
	gtk_widget_show (tmp_image);
	Stop_button = (GtkWidget*) gtk_tool_button_new (tmp_image, "Stop");
	gtk_widget_set_sensitive (Stop_button,FALSE);
	gtk_widget_show (Stop_button);
	gtk_tool_item_set_homogeneous (GTK_TOOL_ITEM ( Stop_button ), FALSE);
	gtk_container_add (GTK_CONTAINER (toolbar1), Stop_button);

	vbox_body = gtk_vbox_new (FALSE, 2);
	gtk_widget_show (vbox_body);
	gtk_container_set_border_width (GTK_CONTAINER(vbox_body), 2);
	gtk_box_pack_start(GTK_BOX(vbox1), vbox_body, TRUE, TRUE, 0);

	/* Create the notebook widget */
	notebook = GTK_NOTEBOOK(gtk_notebook_new() );
	gtk_box_pack_start (GTK_BOX(vbox_body), GTK_WIDGET(notebook),TRUE,TRUE,0);
	gtk_notebook_set_tab_pos (notebook, GTK_POS_TOP);
	gtk_notebook_set_scrollable (notebook, TRUE);
	gtk_notebook_popup_enable (notebook);
	gtk_widget_show (GTK_WIDGET(notebook));

  /*	hbox_sb = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (hbox_sb);
	gtk_box_pack_end (GTK_BOX (vbox_body), hbox_sb, FALSE, TRUE, 0);*/

/*	viewport1 = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport1);
	gtk_box_pack_start (GTK_BOX (hbox_sb), viewport1, TRUE, TRUE, 0);*/

/*	ebox = gtk_event_box_new();
	pad_image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_AUTHENTICATION, GTK_ICON_SIZE_MENU);
	gtk_widget_show (pad_image);
	gtk_container_add (GTK_CONTAINER(ebox), pad_image);
	gtk_widget_show (ebox);
	gtk_widget_set_size_request(ebox, 15, -1);*/

#ifdef GNM_USE_HILDON
	hildon_window_add_toolbar(HILDON_WINDOW (MainWindow), GTK_TOOLBAR(toolbar1));

	/* Menu for Add button*/
	addMenu = hildon_gtk_menu_new ();
	addFileM = gtk_menu_item_new_with_label ("Add a file");
	g_signal_connect (G_OBJECT (addFileM), "activate", G_CALLBACK (addFileM_cb), addMenu);
	gtk_menu_shell_append (GTK_MENU_SHELL (addMenu), addFileM);
	addFolderM = gtk_menu_item_new_with_label ("Add a folder");
	g_signal_connect (G_OBJECT (addFolderM), "activate", G_CALLBACK (addFolderM_cb), addMenu);
	gtk_menu_shell_append (GTK_MENU_SHELL (addMenu), addFolderM);
	gtk_widget_show_all (addMenu);

	/* Connect */
	g_signal_connect (G_OBJECT (AddFile_button), "tap-and-hold", NULL, NULL);
	gtk_widget_tap_and_hold_setup (AddFile_button, addMenu, NULL, 0);

	/* Menu for Extract button*/
	extractMenu = hildon_gtk_menu_new ();
	extractEverythingM = gtk_menu_item_new_with_label ("Extract everything");
	g_signal_connect (G_OBJECT (extractEverythingM), "activate", G_CALLBACK (extractEverythingM_cb), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (extractMenu), extractEverythingM);
	extractSelectedM = gtk_menu_item_new_with_label ("Extract selected files");
	g_signal_connect (G_OBJECT (extractSelectedM), "activate", G_CALLBACK (extractSelectedM_cb), NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (extractMenu), extractSelectedM);
	gtk_widget_show_all (extractMenu);

	/* Connect */
	g_signal_connect (G_OBJECT (Extract_button), "tap-and-hold", NULL, NULL);
	gtk_widget_tap_and_hold_setup (Extract_button, extractMenu, NULL, 0);

#endif

#ifndef GNM_USE_HILDON
	g_signal_connect ((gpointer) new1, "activate", G_CALLBACK (am_a_new_archive), NULL);
	g_signal_connect ((gpointer) open1, "activate", G_CALLBACK (am_a_open_archive), NULL);
	g_signal_connect ((gpointer) extract_menu, "activate", G_CALLBACK (am_a_extract_archive), NULL);
	g_signal_connect ((gpointer) addfile, "activate", G_CALLBACK (am_a_add_files_archive), NULL);
	g_signal_connect ((gpointer) select_all, "activate", G_CALLBACK (am_a_select_all), NULL);
	g_signal_connect ((gpointer) deselect_all, "activate", G_CALLBACK (am_a_deselect_all), NULL);
	g_signal_connect ((gpointer) close1, "activate", G_CALLBACK (am_a_close_archive), NULL);
	g_signal_connect ((gpointer) quit1, "activate", G_CALLBACK (gui_cb_application_quit), NULL);
	g_signal_connect ((gpointer) delete_menu, "activate", G_CALLBACK (am_a_delete_archive), NULL);
	g_signal_connect ((gpointer) tmp_dir, "activate", G_CALLBACK (am_a_create_settings_window), NULL);
	g_signal_connect ((gpointer) about1, "activate", G_CALLBACK (am_a_about), NULL);
	g_signal_connect ((gpointer) view_shell_output1, "activate", G_CALLBACK (am_a_show_cmd_line_output), NULL);

	g_signal_connect ((gpointer) New_button, "clicked", G_CALLBACK (am_a_new_archive), NULL);
	g_signal_connect ((gpointer) Open_button, "clicked", G_CALLBACK (am_a_open_archive), NULL);
#endif
	g_signal_connect ((gpointer) AddFile_button, "clicked", G_CALLBACK (am_a_add_files_archive), NULL);
  	g_signal_connect ((gpointer) Extract_button, "clicked", G_CALLBACK (am_a_extract_archive), NULL);
	g_signal_connect ((gpointer) Delete_button, "clicked", G_CALLBACK (am_a_delete_archive), NULL);
	g_signal_connect ((gpointer) Stop_button, "clicked", G_CALLBACK (am_a_cancel_archive), NULL);
	g_signal_connect (MainWindow, "key-press-event", NULL, NULL);

	gtk_window_add_accel_group (GTK_WINDOW (MainWindow), accel_group);

#ifdef GNM_USE_HILDON
	if(smode == 0)
		hildon_gtk_window_set_portrait_flags (GTK_WINDOW (MainWindow),HILDON_PORTRAIT_MODE_SUPPORT);
	else if(smode == 1)
		hildon_gtk_window_set_portrait_flags (GTK_WINDOW (MainWindow),HILDON_PORTRAIT_MODE_REQUEST);

	gtk_widget_hide(closeArchiveB);
#endif
	
	return MainWindow;
}

/**
 * Creates a new page and adds it to the MainWindow's notebook
 *
 * @param archive Should contain AM_Archive type of data
 */
void am_a_add_page (AM_Archive *archive)
{
	GtkWidget *page_hbox, *label, *tab_label, *close_button, *image;
	gchar *filename_only;

	if (gtk_notebook_get_current_page(notebook) > -1)
		gtk_notebook_set_show_tabs (notebook,TRUE);
	else
		gtk_notebook_set_show_tabs (notebook,FALSE);

#ifdef GNM_USE_HILDON
	archive->scrollwindow = hildon_pannable_area_new ();
	//g_object_set(archive->scrollwindow, "mov-mode", HILDON_MOVEMENT_MODE_BOTH);
#else
	archive->scrollwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW (archive->scrollwindow) , GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
#endif

	gtk_widget_show (archive->scrollwindow);

	page_hbox = gtk_hbox_new(FALSE, 2);

	filename_only = g_strrstr ( archive->path, "/" );
	if (filename_only != NULL)
	{
		filename_only++;
		label = gtk_label_new (filename_only);
		tab_label = gtk_label_new (filename_only);
	}
	else
	{
		label = gtk_label_new (archive->path);
		tab_label = gtk_label_new (archive->path);
	}

	gtk_label_set_max_width_chars(GTK_LABEL(label), 50);
	gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_START);
	gtk_box_pack_start(GTK_BOX(page_hbox), label, FALSE, FALSE, 0);

	gtk_misc_set_alignment(GTK_MISC(tab_label), 0.0, 0);
	gtk_notebook_append_page_menu (notebook, archive->scrollwindow, page_hbox, tab_label);
	gtk_notebook_set_current_page(notebook, -1);

#ifdef GNM_USE_HILDON
	archive->treeview = hildon_gtk_tree_view_new (HILDON_UI_MODE_EDIT);
#else
	archive->treeview = gtk_tree_view_new ();
#endif
	gtk_container_add (GTK_CONTAINER (archive->scrollwindow), archive->treeview);
	gtk_widget_show (archive->treeview);
	gtk_tree_view_set_rules_hint ( GTK_TREE_VIEW (archive->treeview) , TRUE );
	gtk_tree_view_set_search_equal_func (GTK_TREE_VIEW (archive->treeview),(GtkTreeViewSearchEqualFunc) treeview_select_search, NULL, NULL);
	GtkTreeSelection *sel = gtk_tree_view_get_selection( GTK_TREE_VIEW (archive->treeview) );
	gtk_tree_selection_set_mode(sel, GTK_SELECTION_MULTIPLE);
}

/**
 * Returns an id of the new archive
 *
 * @return Returns id
 */
gint am_a_get_new_archive_idx()
{
	gint i;

	for(i = 0; i < 1024; i++)
	{
		if (archive[i] == NULL)
			return i;
	}
	return -1;
}

/**
 * Returns an id of the current archive
 *
 * @return Returns id
 */
gint am_a_find_archive_index (gint page_num)
{
	GtkWidget *scrollwindow;
	gint i;

	scrollwindow = gtk_notebook_get_nth_page(notebook, page_num);
	for (i = 0; i < 1024; i++)
	{
		if (archive[i] != NULL && archive[i]->scrollwindow == scrollwindow)
			return i;
	}
	return -1;
}

/**
 * Sets the labels text
 *
 */
void set_label (GtkWidget *label , gchar *text)
{
    gchar *tmp_markup = g_strdup_printf ("<b>%s</b>",text );
    gtk_label_set_markup ( GTK_LABEL (label) , tmp_markup);
    g_free (tmp_markup);
}
