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

#ifndef BZIP2_H
#define BZIP2_H

#include <gtk/gtk.h>
#include "gui_cb.h"
#include "gui.h"
#include "main.h"
#include "common_archive.h"

/**
 * Function used to opening bzip2 archives
 *
 * @param archive Archive we want to open
 *
 */
void OpenBzip2 ( AM_Archive *archive );

/**
 * Internal function, used by the others to extract gzip or bzip2 archive
 *
 * @param archive Should contain AM_Archive type of data
 * @param flag Tells us archive type; TRUE - gzip, otherwise - bzip2
 *
 */
void gzip_bzip2_extract ( AM_Archive *archive , gboolean flag );

/**
 * Internal function, used by the others to add or delete a list of elements from archive
 *
 * @param list List of elements, which should be added or deleted
 * @param archive Should contain AM_Archive type of data
 * @param dummy Tells us archive type; TRUE - gzip, otherwise - bzip2
 * @param add I TRUE then elements will be added, otherwise removed
 * @return TRUE if succeed, otherwise FAILED
 *
 */
void am_add_delete_tar_bzip2_gzip ( GString *list , AM_Archive *archive , gboolean dummy , gboolean add );

GChildWatchFunc *AddToTar (GPid pid,gint status , gpointer data);
gboolean file_extension_is (const char *filename, const char *ext);

#endif /* BZIP2_H */
