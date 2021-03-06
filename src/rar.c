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
 * Support for rar archives.
 */

#include "config.h"
#include "rar.h"

extern gboolean unrar;
static gboolean RarOpen (GIOChannel *ioc, GIOCondition cond, gpointer data);
GtkTreeIter iter;

/**
 * Function used to opening rar archives
 *
 * @param archive Archive we want to open
 *
 */
void OpenRar ( AM_Archive *archive )
{
	jump_header = FALSE;
	gchar *command = NULL;
	gchar *rar = NULL;

	if (unrar)
		rar = "unrar";
	else
		rar = "rar";

	command = g_strconcat ( rar," vl -c- " , archive->escaped_path, NULL );
	archive->dummy_size = 0;
    	archive->nr_of_files = 0;
    	archive->nr_of_dirs = 0;
	archive->parse_output = RarOpen;
	archive->format ="RAR";
	SpawnAsyncProcess ( archive , command , 0, 0);
	g_free ( command );
	if ( archive->child_pid == 0 )
		return;

	char *names[]	= {("Filename"),("Date"),("Time")};
	GType types[]= {G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING};
	am_a_create_liststore ( 3, names , (GType *)types, archive );
}

/**
 * Internal function, used by other functions to read archive
 *
 * @param data Should contain AM_Archive type of data
 * @return TRUE if succeed, otherwise FAILED
 *
 */
static gboolean RarOpen (GIOChannel *ioc, GIOCondition cond, gpointer data)
{
	indicateProcessingData(TRUE);
	AM_Archive *archive = data;
	gchar **fields = NULL;
	gchar *line = NULL;
	GIOStatus status = G_IO_STATUS_NORMAL;

	if (cond & (G_IO_IN | G_IO_PRI) )
	{
		do
		{
			/* To avoid RAR's copyright message */
			if (jump_header == FALSE )
			{
				status = g_io_channel_read_line ( ioc, &line, NULL, NULL, NULL );
				if (line == NULL)
					break;
				if  (strncmp (line , "--------" , 8) == 0)
				{
					jump_header = TRUE;
					odd_line = TRUE;
				}
				g_free (line);
				break;
			}
			if ( jump_header && odd_line )
			{
				/* Read the filename */
				status = g_io_channel_read_line ( ioc, &line, NULL, NULL, NULL );
				if ( line == NULL )
					break;
				/* To avoid inserting in the list the last line of the output */
				if (strncmp (line, "--------", 8) == 0 || strncmp (line, "\x0a",1) == 0)
				{
					g_free (line);
					status = g_io_channel_read_line ( ioc, &line, NULL, NULL, NULL );
					g_free (line);
					break;
				}
				gtk_list_store_append (archive->liststore, &iter);
				line[ strlen(line) - 1 ] = '\000';

				/* This to avoid the white space before the filename */
				line++;
				gtk_list_store_set (archive->liststore, &iter,0,line,-1);
				/* Restore the pointer before freeing it */
				line--;
				g_free (line);
				odd_line = ! odd_line;
				break;
			}
			else
			{
				/* Read the rest of the data */
				status = g_io_channel_read_line ( ioc, &line, NULL, NULL, NULL );
				if ( line == NULL)
					break;
				fields = split_line (line,9);
				if (fields[5] == NULL)
					break;
				if ( strstr (fields[5] , "d") == NULL && strstr (fields[5] , "D") == NULL )
					archive->nr_of_files++;
				else
					archive->nr_of_dirs++;
				for (x = 0; x < 9; x++)
				{
					if (x == 0 || x == 1)
						gtk_list_store_set (archive->liststore, &iter,x+1,strtoll(fields[x],NULL,0),-1);
					else
						gtk_list_store_set (archive->liststore, &iter,x+1,fields[x],-1);
				}
				while ( gtk_events_pending() )
					gtk_main_iteration();
				archive->dummy_size += strtoll(fields[0],NULL,0);
				g_strfreev ( fields );
				g_free (line);
				odd_line = ! odd_line;
			}
		}
		while (status == G_IO_STATUS_NORMAL);

		if (status == G_IO_STATUS_ERROR || status == G_IO_STATUS_EOF)
			goto done;
	}
	else if (cond & (G_IO_ERR | G_IO_HUP | G_IO_NVAL) )
	{
done:	g_io_channel_shutdown ( ioc,TRUE,NULL );
		g_io_channel_unref (ioc);
		gtk_tree_view_set_model (GTK_TREE_VIEW(archive->treeview), archive->model);
		g_object_unref (archive->model);
		indicateProcessingData(FALSE);
		return FALSE;
	}
	indicateProcessingData(FALSE);
	return TRUE;
}
