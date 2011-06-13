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

#ifndef SEVENZIP_H
#define SEVENZIP_H

#include <gtk/gtk.h>
#include "gui_cb.h"
#include "gui.h"
#include "common_archive.h"

/**
 * Function used to opening 7-zip archives
 *
 * @param archive Archive we want to open
 *
 */
void Open7Zip ( AM_Archive *archive );
#endif
