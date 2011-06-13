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
 * Support for bzip2 archives.
 */

#include "bzip2.h"
#include "main.h"

extern int output_fd;
extern gboolean cli;
extern gboolean TarOpen (GIOChannel *ioc, GIOCondition cond, gpointer data);
short int l;

/**
 * Function used to opening bzip2 archives
 *
 * @param archive Archive we want to open
 *
 */
void OpenBzip2 ( AM_Archive *archive )
{
	indicateProcessingData(TRUE);
	gchar *command;
    if ( g_str_has_suffix ( archive->escaped_path , ".tar.bz2") || g_str_has_suffix ( archive->escaped_path , ".tar.bz") || g_str_has_suffix ( archive->escaped_path , ".tbz") || g_str_has_suffix ( archive->escaped_path , ".tbz2" ) )
	{
		gchar *tar;

		tar = g_find_program_in_path ("gtar");
		if (tar == NULL)
			tar = g_strdup ("tar");

		command = g_strconcat (tar, " tfjv " , archive->escaped_path, NULL );
	g_printf("\n\nEP: %s \n\n", command);
		archive->dummy_size = 0;
		archive->nr_of_files = 0;
		archive->nr_of_dirs = 0;
		archive->format = "TAR.BZIP2";
		archive->parse_output = TarOpen;
		SpawnAsyncProcess ( archive , command , 0, 0);

		g_free (command);
		g_free (tar);

		if ( archive->child_pid == 0 )
			return;

		char *names[]= {("Filename"),("Permissions"),("Symbolic Link"),("Owner/Group"),("Size"),("Date"),("Time")};
		GType types[]= {G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_UINT64,G_TYPE_STRING,G_TYPE_STRING};
		am_a_create_liststore ( 7, names , (GType *)types, archive);
		archive->type = AM_ARCHIVETYPE_TAR_BZ2;
	}
	else
	{
		extract_window = am_a_create_extract_dialog ( 0 , archive);
		command = am_a_parse_extract_dialog_options ( archive , extract_window, NULL );
		gtk_widget_destroy ( extract_window->dialog1 );
		g_free (extract_window);
	}
	indicateProcessingData(FALSE);
}

/**
 * Internal function, used by the others to extract gzip or bzip2 archive
 *
 * @param archive Should contain AM_Archive type of data
 * @param flag Tells us archive type; TRUE - gzip, otherwise - bzip2
 *
 */
void gzip_bzip2_extract ( AM_Archive *archive , gboolean flag )
{
    	gchar *text = NULL;
	gchar *filename_only = NULL;
	gchar *command = NULL;
	gboolean result = FALSE;
	gboolean ext;

	if ( ! cli )
		archive->extraction_path = g_strdup (gtk_entry_get_text ( GTK_ENTRY (extract_window->destination_path_entry) ));

	if ( strlen ( archive->extraction_path ) > 0 )
	{
		if (! cli)
		{
			if (flag)
				text = g_strdup_printf("Extracting gzip file to %s", archive->extraction_path);
			else
				text = g_strdup_printf("Extracting bzip2 file to %s", archive->extraction_path);
			//Print somewhere 'text' variable
			g_free (text);
		}

		filename_only = g_strrstr (archive->escaped_path , "/");
		if (file_extension_is (filename_only,".gz") || file_extension_is (filename_only,".bz2") )
			ext = TRUE;
		else
			ext = FALSE;

//#ifdef GNM_NUSE_TMP
		if (ext)
			command = g_strconcat ("cp -f ", archive->escaped_path, g_strconcat(" ", whereToExtract, NULL), NULL);
		else
			command = g_strconcat ("cp -f ", archive->escaped_path, g_strconcat(" ", whereToExtract, NULL) , filename_only, flag ? ".gz" : ".bz2", NULL);

		g_printf("\n\nCommand0: %s \n\n", command);
/*#else
		if (ext)
			command = g_strconcat ("cp -f ", archive->escaped_path, " /tmp", NULL);
		else
			command = g_strconcat ("cp -f ", archive->escaped_path, " /tmp" , filename_only, flag ? ".gz" : ".bz2", NULL);
#endif*/

		result = am_a_run_command (command , 0);
		g_free (command);
		if (result == 0)
			return ;
//#ifdef GNM_NUSE_TMP
		if ( ext  )
			command = g_strconcat (flag ? "gzip -f -d -n " : "bzip2 -f -d ", whereToExtract,filename_only, NULL);
		else
			command = g_strconcat (flag ? "gzip -f -d -n " : "bzip2 -f -d ",whereToExtract,filename_only, flag ? ".gz" : ".bz2", NULL);
	g_printf("\n\nCommand0A: %s \n\n", command);
/*#else
		if ( ext  )
			command = g_strconcat (flag ? "gzip -f -d -n " : "bzip2 -f -d ", "/tmp",filename_only, NULL);
		else
			command = g_strconcat (flag ? "gzip -f -d -n " : "bzip2 -f -d ","/tmp",filename_only, flag ? ".gz" : ".bz2", NULL);
#endif*/

		result = am_a_run_command (command , 0);
		g_free (command);
		if (result == 0)
			return;

		if (ext)
		{
			if (flag)
				filename_only[strlen(filename_only) - 3] = '\0';
			else
				filename_only[strlen(filename_only) - 4] = '\0';
//#ifdef GNM_NUSE_TMP
				command = g_strconcat (g_strconcat("mv -f ", whereToExtract, NULL), filename_only, " ", archive->extraction_path,NULL);
				g_printf("\n\nCommandA2: %s \n\n", command);
/*#else
				command = g_strconcat ("mv -f /tmp",filename_only, " ", archive->extraction_path,NULL);
#endif*/
		}
		else
		{
//#ifdef GNM_NUSE_TMP
			if ( g_file_test (archive->extraction_path, G_FILE_TEST_IS_DIR) )
				command = g_strconcat (g_strconcat("mv -f ", whereToExtract, NULL), filename_only, " ", archive->extraction_path,filename_only,NULL);
			else
				command = g_strconcat (g_strconcat("mv -f ", whereToExtract, NULL), filename_only, " ", archive->extraction_path,NULL);

	g_printf("\n\nCommandA3: %s \n\n", command);
/*#else
			if ( g_file_test (archive->extraction_path, G_FILE_TEST_IS_DIR) )
				command = g_strconcat ("mv -f /tmp",filename_only, " ", archive->extraction_path,filename_only,NULL);
			else
				command = g_strconcat ("mv -f /tmp",filename_only, " ", archive->extraction_path,NULL);
#endif*/
		}

		result = am_a_run_command (command , 0);
		g_free (command);
		if (result == 0)
			return;
	}
	if (result == 0)
	{
		archive->status = AM_ARCHIVESTATUS_IDLE;
		gtk_widget_hide ( viewport2 );
		gtk_widget_set_sensitive (Stop_button, FALSE);
	}
	else
		am_a_watch_child (archive->child_pid, 0, archive);
}

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

void am_add_delete_tar_bzip2_gzip ( GString *list , AM_Archive *archive , gboolean dummy , gboolean add )
{
	gchar *command, *msg, *tar,*temp_name,*file_ext;
	gboolean result;

	if ( ! cli )
	{
		gtk_widget_show (viewport2);
		msg = g_strdup_printf("Decompressing tar file...");
		//Print somewhere 'msg' variable
		g_free (msg);
	}

//#ifdef GNM_NUSE_TMP
	temp_name = g_strconcat ( g_strconcat(" ", whereToExtract, NULL), g_strrstr (archive->escaped_path , "/"), NULL);
	g_printf("\n\ntemp_name: %s \n\n", temp_name);
/*#else
	temp_name = g_strconcat ( " /tmp", g_strrstr (archive->escaped_path , "/"), NULL);
#endif*/
	command = g_strconcat ("cp -ar " ,archive->escaped_path,temp_name,NULL);
	g_printf("\n\nCommand: %s \n\n", command);

	if ( ! cli)
		result = am_a_run_command (command , 0);
	else
		result = SpawnSyncCommand ( command );
	g_free (command);

	if (result == 0)
	{
		g_free (temp_name);
		return;
	}
	command = g_strconcat (dummy ? "gzip -f " : "bzip2 ", "-f -d ",temp_name,NULL);
	g_printf("\n\nCommand2: %s \n\n", command);

	if ( ! cli )
		result = am_a_run_command (command , 0);
	else
		result = SpawnSyncCommand ( command );

	g_free (command);

	if (result == 0)
	{
		g_free (temp_name);
		return;
	}

	tar = g_find_program_in_path ("gtar");

	if (tar == NULL)
		tar = g_strdup ("tar");

	l = strlen (temp_name);
	g_printf("\n\ntemp_name2: %s \n\n", temp_name);

	if (file_extension_is (archive->escaped_path,".tar.bz2") )
		temp_name[l - 4] = 0;
	else if (file_extension_is (archive->escaped_path,".tbz2") )
	{
		temp_name[l - 3] = 'a';
		temp_name[l - 2] = 'r';
		temp_name[l - 1] = 0;
	}
	else if (file_extension_is (archive->escaped_path,".tar.gz") )
		temp_name[l - 3] = 0;

	else if (file_extension_is (archive->escaped_path,".tgz") || file_extension_is (archive->escaped_path, ".tbz") )
	{
		temp_name[l - 2] = 'a';
		temp_name[l - 1] = 'r';
	}

	if ( add ){
		command = g_strconcat (tar, " ",
							"-uvvf ",
							temp_name,
							list->str , NULL );
		g_printf("\n\Add: %s \n\n", command);
	}
	else
		command = g_strconcat (tar, " --delete -f " , temp_name , list->str , NULL );

	if ( ! cli)
		result = am_a_run_command (command , 0);
	else
		result = SpawnSyncCommand ( command );

	g_free (command);
	g_free (tar);

	if (result == 0)
	{
		g_free (temp_name);
		return;
	}

	if ( ! cli )
	{
		msg = g_strdup_printf("Recompressing tar file with...");
		//Print somewhere 'msg' variable
		g_free (msg);
	}

	command = g_strconcat ( dummy ? "gzip -f " : "bzip2 ", "-f " , temp_name , NULL );
	g_printf("\n\nCommandD1: %s \n\n", command);

	if ( ! cli )
		result = am_a_run_command (command , 0);
	else
		result = SpawnSyncCommand ( command );

	g_free (command);

	if (result == 0)
	{
		g_free (temp_name);
		return;
	}

	if (dummy)
		file_ext = ".gz";
	else
		file_ext = ".bz2";

	// Move archive from /tmp to selected destination
	command = g_strconcat ( "mv " , temp_name , file_ext, " " ,archive->escaped_path, NULL );
	g_printf("\n\nMove: %s \n\n", command);

	if ( ! cli )
		result = am_a_run_command (command , 1);
	else
		result = SpawnSyncCommand ( command );
	g_free (command);
	g_free (temp_name);

	if (result == 0)
		return;
}
