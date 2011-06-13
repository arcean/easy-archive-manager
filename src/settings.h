/*
 *  Copyright (c) 2010, 2011 Tomasz Pieniążek <t.pieniazek@gazeta.pl>
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <gtk/gtk.h>
#include <glib.h>

#include "config.h"
#include "gui.h"
#include "gui_cb.h"
#include "common_archive.h"
#include "add.h"

#ifdef GNM_USE_HILDON
#include <hildon/hildon.h>
#include <hildon/hildon-program.h>
#endif

GtkWidget *settingsWindow;
GtkWidget *sBox;
GtkWidget *textEntry;
GtkWidget *saveButton;
GtkWidget *saveLabel;

#ifdef GNM_USE_HILDON
	GtkWidget *screenMode;
	GtkWidget *screenModeButton;
#endif

void am_a_create_settings_window ();
void am_a_save_tmpdir_cb ();



#endif
