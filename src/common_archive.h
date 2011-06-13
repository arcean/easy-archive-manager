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

#ifndef COMMON_ARCHIVE_H
#define COMMON_ARCHIVE_H

typedef enum
{
	AM_ARCHIVETYPE_UNKNOWN,
	AM_ARCHIVETYPE_7ZIP,
	AM_ARCHIVETYPE_DEB,
	AM_ARCHIVETYPE_BZIP2,
	AM_ARCHIVETYPE_GZIP,
	AM_ARCHIVETYPE_ISO,
	AM_ARCHIVETYPE_RAR,
	AM_ARCHIVETYPE_RPM,
	AM_ARCHIVETYPE_TAR,
	AM_ARCHIVETYPE_TAR_BZ2,
	AM_ARCHIVETYPE_TAR_GZ,
	AM_ARCHIVETYPE_ZIP,
} AM_ArchiveType;

typedef enum
{
	AM_ARCHIVESTATUS_IDLE,
	AM_ARCHIVESTATUS_EXTRACT,
	AM_ARCHIVESTATUS_ADD,
	AM_ARCHIVESTATUS_DELETE,
	AM_ARCHIVESTATUS_OPEN,
} AM_ArchiveStatus;

typedef struct _AM_Archive AM_Archive;

struct _AM_Archive
{
	AM_ArchiveType type;
	AM_ArchiveStatus status;
	gchar *path;
	gchar *escaped_path;
	gchar *tmp;
	gchar *format;
	gchar *extraction_path;
	gint nr_of_files;
	gint nr_of_dirs;
	GPid child_pid;
	GtkWidget *scrollwindow;
	GtkListStore *liststore;
	GtkTreeModel *model;
	GtkWidget *treeview;
	unsigned long long int dummy_size;
	gboolean ticker;
	gboolean (*parse_output) (GIOChannel *ioc, GIOCondition cond, gpointer data);
};
// ISO things:
gchar *system_id, *volume_id, *publisher_id, *preparer_id, *application_id, *creation_date;
gchar *modified_date, *expiration_date, *effective_date;

gint input_fd, output_fd, error_fd;
AM_Archive *archive[1024];
AM_Archive *archive_cmd;
unsigned short int x;

#endif /* COMMON_ARCHIVE_H */
