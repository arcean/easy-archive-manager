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
 * Support for gzip archives.
 */

#include "bzip2.h"
#include "gzip.h"

FILE *fd;
extern int output_fd;

/**
 * Function used to opening gzip archives
 *
 * @param archive Archive we want to open
 *
 */
void OpenGzip ( AM_Archive *archive )
{
	gchar *command;

	if ( g_str_has_suffix ( archive->escaped_path , ".tar.gz") || g_str_has_suffix ( archive->escaped_path , ".tgz") )
	{
    gchar *tar;

    tar = g_find_program_in_path ("gtar");
    if (tar == NULL)
      tar = g_strdup ("tar");

		command = g_strconcat (tar, " tzvf " , archive->escaped_path, NULL );
		archive->dummy_size = 0;
		archive->nr_of_files = 0;
		archive->nr_of_dirs = 0;
		archive->format ="TAR.GZIP";
		archive->parse_output = TarOpen;

		SpawnAsyncProcess ( archive , command , 0, 0);

		g_free (command);
		g_free (tar);

		if ( archive->child_pid == 0 )
			return;

		char *names[]= {("Filename"),("Permissions"),("Symbolic Link"),("Owner/Group"),("Size"),("Date"),("Time")};
		GType types[]= {G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_UINT64,G_TYPE_STRING,G_TYPE_STRING};
		am_a_create_liststore ( 7, names , (GType *)types, archive );
        	archive->type = AM_ARCHIVETYPE_TAR_GZ;
	}
	else
	{
		extract_window = am_a_create_extract_dialog ( 0 , archive);
		command = am_a_parse_extract_dialog_options ( archive , extract_window, NULL );
		gtk_widget_destroy ( extract_window->dialog1 );
		g_free (extract_window);
	}
}


