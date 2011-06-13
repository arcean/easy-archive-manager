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
 * Declares AM_Archive type.
 * Contains functions to create and destroy AM_Archive.
 *
 * AM_Archive is basic structure in our application.
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <sys/wait.h>
#include "common_archive.h"
#include "gui_cb.h"

extern const gchar *locale;
extern gboolean am_a_report_child_stderr (GIOChannel *ioc, GIOCondition cond, gpointer data);

/**
 * Creates new AM_Archive
 *
 * @return Returns new AM_Archive
 */
AM_Archive *am_a_init_archive_structure()
{
	AM_Archive *archive = NULL;
	archive = g_new0(AM_Archive,1);
	archive->ticker = FALSE;
	return archive;
}

/**
 * Spawn a new process
 *
 * @param archive Should contain AM_Archive type of data
 * @param command Command, which we want to execute
 * @param input TRUE If we want to use our standard input, oterwise will be used /dev/null
 *
 */
void SpawnAsyncProcess(AM_Archive *archive, gchar *command, gboolean input, gboolean output_flag)
{
	GIOChannel *ioc , *err_ioc, *out_ioc;
	GError *error = NULL;
	gchar **argv;
	gint argcp, response;

	g_shell_parse_argv ( command , &argcp , &argv , NULL);
	if ( ! g_spawn_async_with_pipes (
		NULL,
		argv,
		NULL,
		G_SPAWN_SEARCH_PATH | G_SPAWN_DO_NOT_REAP_CHILD,
		NULL,
		NULL,
		&archive->child_pid,
		input ? &input_fd : NULL,
		&output_fd,
		&error_fd,
		&error) )
	{
		response = ShowGtkMessageDialog (NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, "Can't run the archiver executable:",error->message);
		g_error_free (error);
		g_strfreev ( argv );
		archive->child_pid = 0;
		return;
	}
	g_strfreev ( argv );

	if ( archive->parse_output )
	{
		ioc = g_io_channel_unix_new ( output_fd );
		g_io_channel_set_encoding (ioc, locale , NULL);
		g_io_channel_set_flags ( ioc , G_IO_FLAG_NONBLOCK , NULL );
		g_io_add_watch (ioc, G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL, archive->parse_output, archive);
		g_child_watch_add ( archive->child_pid, (GChildWatchFunc)am_a_watch_child, archive);
	}
	if (output_flag)
	{
		out_ioc = g_io_channel_unix_new ( output_fd );
		g_io_channel_set_encoding (out_ioc, locale , NULL);
		g_io_channel_set_flags ( out_ioc , G_IO_FLAG_NONBLOCK , NULL );
		g_io_add_watch (out_ioc, G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL, am_a_report_child_stderr, NULL);
	}

	err_ioc = g_io_channel_unix_new ( error_fd );
	g_io_channel_set_encoding (err_ioc, locale , NULL);
	g_io_channel_set_flags ( err_ioc , G_IO_FLAG_NONBLOCK , NULL );
	g_io_add_watch (err_ioc, G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL, am_a_report_child_stderr, NULL);
}

/**
 * Removes an AM_Archive
 *
 * @param archive Should contain AM_Archive type of data
 *
 */
void am_a_clean_archive_structure(AM_Archive *archive)
{
	if (archive == NULL)
		return;

	if(archive->path != NULL)
	{
		g_free(archive->path);
		archive->path = NULL;
	}

	if(archive->escaped_path != NULL)
	{
		g_free(archive->escaped_path);
		archive->escaped_path = NULL;
	}

	if (archive->tmp != NULL)
	{
		if (strstr(archive->tmp , "/tmp" ) )
			unlink(archive->tmp);

		g_free(archive->tmp);
		archive->tmp = NULL;
	}

	if (archive->extraction_path != NULL )
		{
			if (strcmp(archive->extraction_path , "/tmp/") != 0)
				g_free(archive->extraction_path);
		}
		
	g_free (archive);
}
