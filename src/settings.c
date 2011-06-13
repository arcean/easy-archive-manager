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

void am_a_save_tmpdir_cb () {
	whereToExtract = g_strdup(gtk_entry_get_text(textEntry));
	if ((whereToExtract == NULL) || (whereToExtract == ""))
		return;
	confStoreLocation(whereToExtract);
#ifdef GNM_USE_HILDON
	smode = hildon_picker_button_get_active(screenModeButton);
	hildon_banner_show_information (GTK_WIDGET (settingsWindow), NULL, "Settings saved");
#endif
	gtk_widget_hide(settingsWindow);	
}

/**
 * Creates "Settings" window
 *
 */
void am_a_create_settings_window (){
#ifdef GNM_USE_HILDON
	settingsWindow = hildon_stackable_window_new ();
	textEntry = hildon_entry_new (HILDON_SIZE_AUTO);
	hildon_entry_set_text(textEntry, whereToExtract);
	saveButton = hildon_button_new_with_text (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH, HILDON_BUTTON_ARRANGEMENT_HORIZONTAL, "Save settings", NULL);

	screenMode = hildon_touch_selector_new_text();
	hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (screenMode), "Auto");
	hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (screenMode), "Landscape");
	hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (screenMode), "Portrait");
	hildon_touch_selector_set_active (HILDON_TOUCH_SELECTOR (screenMode), 0, 0);
	screenModeButton = hildon_picker_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH, HILDON_BUTTON_ARRANGEMENT_HORIZONTAL);
	hildon_button_set_title (HILDON_BUTTON (screenModeButton), "Screen Orientation");

	hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (screenModeButton), HILDON_TOUCH_SELECTOR (screenMode));

	
#else
	settingsWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	textEntry = gtk_entry_new ();
	gtk_entry_set_text(textEntry, whereToExtract);
	saveButton = gtk_button_new_with_label ("Save setings");
#endif
	gtk_window_set_title(settingsWindow, "Settings");

	saveLabel = gtk_label_new("Temporary catalog: ");

	sBox = gtk_vbox_new (FALSE, 6);
	gtk_container_add(GTK_CONTAINER(settingsWindow), sBox);
	gtk_box_pack_start(sBox, saveLabel, FALSE, FALSE, 6);
	gtk_box_pack_start(sBox, textEntry, FALSE, FALSE, 6);
#ifdef GNM_USE_HILDON
	gtk_box_pack_start(sBox, screenModeButton, FALSE, FALSE, 6);	
#endif
	gtk_box_pack_start(sBox, saveButton, FALSE, FALSE, 6);

	g_signal_connect_after (saveButton, "clicked", G_CALLBACK (am_a_save_tmpdir_cb), NULL);

#ifdef GNM_USE_HILDON
	if(smode == 0)
		hildon_gtk_window_set_portrait_flags (GTK_WINDOW (settingsWindow),HILDON_PORTRAIT_MODE_SUPPORT);
	else if(smode == 1)
		hildon_gtk_window_set_portrait_flags (GTK_WINDOW (settingsWindow),HILDON_PORTRAIT_MODE_REQUEST);
#endif
	
	gtk_widget_show_all(GTK_WIDGET(settingsWindow));
}



