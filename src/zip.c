/**
 * @file
 * @author  Tomasz Pieniążek (2010, 2011) <t.pieniazek@gazeta.pl>
 * 	    Giuseppe Torelli (2005, 2006)- <colossus73@gmail.com>
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
 * Support for zip archives.
 */

#include "zip.h"
#include "utils.h"

static gboolean ZipOpen (GIOChannel *ioc, GIOCondition cond, gpointer data);

/**
 * Function used to opening zip archives
 *
 * @param archive Archive we want to open
 *
 */
void OpenZip (AM_Archive *archive)
{
	gchar *command = g_strconcat ( "unzip -vl -qq " , archive->escaped_path, NULL );
	archive->dummy_size = 0;
    	archive->nr_of_files = 0;
    	archive->nr_of_dirs = 0;
	archive->parse_output = ZipOpen;
	archive->format ="ZIP";

	SpawnAsyncProcess ( archive , command , 0, 0);

	g_free ( command );
	if ( archive->child_pid == 0 )
		return;

	char *names[]= {("Filename"),("Original"),("Method"),("Compressed"),("Ratio"),("Date"),("Time"),("Checksum")};
	GType types[]= {G_TYPE_STRING,G_TYPE_UINT64,G_TYPE_STRING,G_TYPE_UINT64,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING};
	am_a_create_liststore ( 8, names , (GType *)types, archive );
}

/**
 * Internal function, used by other functions to read archive
 *
 * @param data Should contain AM_Archive type of data
 * @return TRUE if succeed, otherwise FAILED
 *
 */
static gboolean ZipOpen (GIOChannel *ioc, GIOCondition cond, gpointer data)
{
	indicateProcessingData(TRUE);
	AM_Archive *archive = data;
	gchar **fields = NULL;
	gchar *filename = NULL;
	gchar *line = NULL;
	GtkTreeIter iter;
	GIOStatus status = G_IO_STATUS_NORMAL;

	if (cond & (G_IO_IN | G_IO_PRI) )
	{
		do
		{
			status = g_io_channel_read_line ( ioc, &line, NULL, NULL, NULL );
			if ( line == NULL )
				break;
			fields = split_line (line , 7);
			filename = get_last_field (line , 8);
			if ( g_str_has_suffix(filename , "/") == TRUE)
				archive->nr_of_dirs++;
			else
				archive->nr_of_files++;
			if ( filename != NULL )
			{
				gtk_list_store_append (archive->liststore, &iter);
				for ( x = 0; x < 7; x++)
				{
					if (x == 0 || x == 2)
						gtk_list_store_set (archive->liststore, &iter,x+1, strtoll (fields[x],NULL,0), -1);
					else
						gtk_list_store_set (archive->liststore, &iter,x+1,fields[x], -1);
				}
				archive->dummy_size += strtoll (fields[0],NULL,0);
				gtk_list_store_set (archive->liststore, &iter,0,filename,-1);
			}

			while (gtk_events_pending())
				gtk_main_iteration();
			g_strfreev ( fields );
			g_free (line);
		}
		while (status == G_IO_STATUS_NORMAL);

		if (status == G_IO_STATUS_ERROR || status == G_IO_STATUS_EOF)
			goto done;
	}
	else if (cond & (G_IO_ERR | G_IO_HUP | G_IO_NVAL))
	{
done:	g_io_channel_shutdown (ioc,TRUE,NULL);
		g_io_channel_unref (ioc);
		gtk_tree_view_set_model (GTK_TREE_VIEW(archive->treeview), archive->model);
		g_object_unref (archive->model);
		indicateProcessingData(FALSE);
		return FALSE;
	}
	indicateProcessingData(FALSE);
	return TRUE;
}
