/**
 * @file
 * @author  Tomasz Pieniążek (2010, 2011) <t.pieniazek@gazeta.pl>
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
 * Settings window.
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include "settings.h"
#include "main.h"
#include "gconf.h"

void am_a_save_tmpdir_cb() {
	//whereToExtract = g_strdup(gtk_entry_get_text(textEntry));
	if ((whereToExtract == NULL) || (whereToExtract == ""))
		return;
	confStoreLocation(whereToExtract);
#ifdef GNM_USE_HILDON
	smode = hildon_picker_button_get_active(HILDON_PICKER_BUTTON (screenModeButton));
	hildon_banner_show_information (GTK_WIDGET (settingsWindow), NULL, "Settings saved");
#endif
}

void am_a_temp_catalog_dialog() {
	static GtkWidget *File_Selector = NULL;
	GtkFileFilter *filter;
	gchar *path = NULL;
	gint response;

	if (File_Selector == NULL)
	{
#ifdef GNM_USE_HILDON
		File_Selector = GTK_FILE_CHOOSER (hildon_file_chooser_dialog_new (MainWindow, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER));
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(File_Selector), whereToExtract);

#else
		File_Selector = gtk_file_chooser_dialog_new ( "Open",
						GTK_WINDOW (MainWindow),
						GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
						GTK_STOCK_CANCEL,
						GTK_RESPONSE_CANCEL,
						"gtk-open",
						GTK_RESPONSE_ACCEPT,
						NULL);
#endif
	}

	response = gtk_dialog_run (GTK_DIALOG (File_Selector));
	if((response == GTK_RESPONSE_ACCEPT) || (response == GTK_RESPONSE_OK))
	{
		whereToExtract = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (File_Selector)); 
		hildon_button_set_value(HILDON_BUTTON(tempButton), whereToExtract);
		am_a_save_tmpdir_cb();
	}

	gtk_widget_hide(File_Selector);
}

/**
 * Creates "Settings" window
 *
 */
void am_a_create_settings_window(){
#ifdef GNM_USE_HILDON
	settingsWindow = hildon_stackable_window_new ();
	tempButton = hildon_button_new_with_text(HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH, HILDON_BUTTON_ARRANGEMENT_HORIZONTAL, "Temporary catalog", whereToExtract);

	screenMode = hildon_touch_selector_new_text();
	hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (screenMode), "Auto");
	hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (screenMode), "Landscape");
	hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (screenMode), "Portrait");
	hildon_touch_selector_set_active (HILDON_TOUCH_SELECTOR (screenMode), 0, 0);
	screenModeButton = hildon_picker_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH, HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
	hildon_button_set_title (HILDON_BUTTON (screenModeButton), "Screen Orientation");

	hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (screenModeButton), HILDON_TOUCH_SELECTOR (screenMode));

#else
	settingsWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL)
#endif
	gtk_window_set_title(GTK_WINDOW(settingsWindow), "Settings");

	saveLabel = gtk_label_new("Temporary catalog: ");

	sBox = gtk_vbox_new (FALSE, 6);
	gtk_container_add(GTK_CONTAINER(settingsWindow), sBox);
	gtk_box_pack_start(sBox, saveLabel, FALSE, FALSE, 6);
	gtk_box_pack_start(sBox, tempButton, FALSE, FALSE, 6);
#ifdef GNM_USE_HILDON
	gtk_box_pack_start(sBox, screenModeButton, FALSE, FALSE, 6);	
#endif

	g_signal_connect_after (tempButton, "clicked", G_CALLBACK (am_a_temp_catalog_dialog), NULL);

#ifdef GNM_USE_HILDON
	if(smode == 0)
		hildon_gtk_window_set_portrait_flags (GTK_WINDOW (settingsWindow),HILDON_PORTRAIT_MODE_SUPPORT);
	else if(smode == 1)
		hildon_gtk_window_set_portrait_flags (GTK_WINDOW (settingsWindow),HILDON_PORTRAIT_MODE_REQUEST);
#endif
	
	gtk_widget_show_all(GTK_WIDGET(settingsWindow));
}



