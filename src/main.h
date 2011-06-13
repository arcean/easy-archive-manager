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

#ifndef MAIN_H
#define MAIN_H

#include <gtk/gtk.h>
#include <glib.h>

#include "config.h"
#include "gui.h"
#include "gui_cb.h"
#include "common_archive.h"
#include "add.h"
#include "gconf.h"

#ifdef GNM_USE_HILDON
#include <hildon/hildon.h>
#include <hildon/hildon-program.h>
#include <libosso.h>
#include <dbus/dbus.h>
#endif

#ifdef GNM_USE_HILDON
	static HildonProgram *hildon_program;
	osso_context_t *osso_context;
	DBusConnection *con;
#endif

#define APP_VERSION "0.5.0"

GtkWidget *MainWindow;
const gchar *locale;
GList *ArchiveSuffix;
GList *ArchiveType;
gboolean addFolder;
gboolean extractSelected;
gchar* whereToExtract;
gint smode; //0 - auto, 1-portrait, 2-landscape

void GetAvailableCompressors();
gchar *get_argv_filename(const gchar *filename);

#endif /* MAIN_H */
