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

#ifndef RPM_H
#define RPM_H

#include <gtk/gtk.h>
#include "gui_cb.h"
#include "gui.h"
#include "common_archive.h"

void OpenRPM ( AM_Archive *archive );
GChildWatchFunc *DecompressCPIO (GPid pid , gint status , gpointer data);
GChildWatchFunc *OpenCPIO (GPid pid , gint status , gpointer data);
gboolean ReadCPIOOutput (GIOChannel *ioc, GIOCondition cond, gpointer data);
gboolean WriteCPIOInput (GIOChannel *ioc, GIOCondition cond, gpointer data);
void CloseChannels ( GIOChannel *ioc );
gboolean ExtractToDifferentLocation (GIOChannel *ioc, GIOCondition cond , gpointer data);
gchar *am_a_open_temp_file ( gchar *temp_path );

#endif
