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
 * Support for DEB packages.
 */

#include "deb.h"
#include "utils.h"

/**
 * Function used to opening deb packages
 *
 * @param archive Archive we want to open
 *
 */
void OpenDeb(AM_Archive *archive)
{
	gchar *command = NULL;
	gchar *archive_no_path = NULL;
	gboolean result;

	archive_no_path = g_strrstr (archive->escaped_path,"/");
	archive_no_path++;

	archive->tmp = g_strconcat (g_strconcat(" ", whereToExtract, "/", NULL),archive_no_path,NULL);

	command = g_strconcat ("cp ",archive->escaped_path,archive->tmp,NULL);

	result = am_a_run_command ( command , 0);
	g_free (command);
	if (result == FALSE)
		return;

	chdir (whereToExtract);

	command = g_strconcat ("ar xv" , archive->tmp, NULL);

	result = am_a_run_command ( command , 0);
	g_free (command);
	g_free (archive->tmp);

	archive->tmp = g_strdup (g_strconcat(whereToExtract, "/data.tar.gz", NULL));

	unlink (g_strconcat(whereToExtract, "/control.tar.gz", NULL));
	unlink (g_strconcat(whereToExtract, "/debian-binary", NULL));

	unlink (archive_no_path);

	if (result == FALSE)
		return;

	command = g_strconcat (g_strconcat("tar tfzv ",whereToExtract, "/data.tar.gz", NULL), NULL);

	archive->dummy_size = 0;
	archive->nr_of_files = 0;
	archive->nr_of_dirs = 0;
	archive->format ="DEB";
	archive->parse_output = TarOpen;
	SpawnAsyncProcess ( archive , command , 0, 0);
	g_free (command);

	if (archive->child_pid == 0)
		return;

	char *names[]= {("Filename"),("Permissions"),("Symbolic Link"),("Owner/Group"),("Size"),("Date"),("Time")};
	GType types[]= {G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_UINT64,G_TYPE_STRING,G_TYPE_STRING};
	am_a_create_liststore ( 7, names , (GType *)types, archive );
}
