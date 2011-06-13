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
 * Creates MainWindow with GUI.
 */

#include "main.h"

gchar *absolute_path = NULL;
gboolean unrar = FALSE;
gint exit_status;
gboolean cli = FALSE;
GError *cli_error = NULL;
gchar *archive_name = NULL;
gchar *_current_dir = NULL;
gchar *extract_path = NULL;
gboolean error_output, file_to_open, ask_and_extract, ask_and_add;
gchar *cli_command = NULL;
extern gchar *current_open_directory;

/**
** Creates a new archive structure, only for cmd line use
**/
AM_Archive *am_a_init_structure_from_cmd_line (char *filename)
{
	AM_Archive *archive_cmd;
	archive_cmd = am_a_init_archive_structure ();
	if (archive_cmd == NULL)
	{
		response = ShowGtkMessageDialog (NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't allocate memory for the archive structure!","" );
		return NULL;
	}

	archive_cmd->path = g_strdup (filename);
	archive_cmd->escaped_path = EscapeBadChars(filename , "$\'`\"\\!?* ()&|@#:;");
	archive_cmd->type = am_a_detect_archive_type ( archive_cmd , NULL );
	if (archive_cmd->type == -2)
		return NULL;
	if ( g_str_has_suffix ( archive_cmd->escaped_path , ".tar.bz2") || g_str_has_suffix ( archive_cmd->escaped_path , ".tar.bz") || g_str_has_suffix ( archive_cmd->escaped_path , ".tbz") || g_str_has_suffix ( archive_cmd->escaped_path , ".tbz2" ) )
		archive_cmd->type = AM_ARCHIVETYPE_TAR_BZ2;
	else if ( g_str_has_suffix ( archive_cmd->escaped_path , ".tar.gz") || g_str_has_suffix ( archive_cmd->escaped_path , ".tgz") )
		archive_cmd->type = AM_ARCHIVETYPE_TAR_GZ;
	return (archive_cmd);
}

gchar *get_argv_filename(const gchar *filename)
{
	gchar *result;

	if (g_path_is_absolute(filename))
		result = g_strdup(filename);
	else
	{
		gchar *cur_dir = g_get_current_dir();
		result = g_strjoin(G_DIR_SEPARATOR_S, cur_dir, filename, NULL);
		g_free(cur_dir);
	}
	return result;
}

void GetAvailableCompressors()
{

	absolute_path = g_find_program_in_path("bzip2");
    if ( absolute_path )
	{
		ArchiveType = g_list_prepend ( ArchiveType, ".bz2");
		ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "*.bz2");
		g_free (absolute_path);
	}

	absolute_path = g_find_program_in_path("ar");
    if ( absolute_path )
	{
	    ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "*.deb");
		g_free (absolute_path);
	}

	absolute_path = g_find_program_in_path("gzip");
	if ( absolute_path )
	{
		ArchiveType = g_list_prepend ( ArchiveType, ".gz");
		ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "*.gz");
		g_free (absolute_path);
	}

	ArchiveType = g_list_prepend ( ArchiveType, ".iso");
	ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "*.iso");

	absolute_path = g_find_program_in_path ("rar");
    if ( absolute_path )
	{
		ArchiveType = g_list_prepend ( ArchiveType, ".rar");
		ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "*.rar");
		g_free (absolute_path);
	}
	else
	{
		absolute_path = g_find_program_in_path ("unrar");
		if ( absolute_path )
		{
			unrar = TRUE;
			ArchiveType = g_list_prepend ( ArchiveType, ".rar");
			ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "*.rar");
			g_free (absolute_path);
		}
	}

	absolute_path = g_find_program_in_path("cpio");
    if ( absolute_path )
	{
	    ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "*.rpm");
		g_free (absolute_path);
	}

	absolute_path = g_find_program_in_path("tar");
	if ( absolute_path )
	{
		ArchiveType = g_list_prepend ( ArchiveType, ".tar");
		ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "*.tar");
		g_free (absolute_path);
		if ( g_list_find ( ArchiveType , ".bz2") )
		{
			ArchiveType = g_list_prepend ( ArchiveType, ".tar.bz2");
			/* The following to avoid double filter when opening */
			ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "");
		}
		if ( g_list_find ( ArchiveType , ".gz") )
		{
			ArchiveType = g_list_prepend ( ArchiveType, ".tar.gz");
			ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "*.tgz");
		}
	}

	absolute_path = g_find_program_in_path ("zip");
    if ( absolute_path )
	{
		ArchiveType = g_list_prepend ( ArchiveType, ".jar");
		ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "*.jar");
		g_free (absolute_path);

		ArchiveType = g_list_prepend ( ArchiveType, ".zip");
		ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "*.zip");
	}

	absolute_path = g_find_program_in_path("7za");
    if ( absolute_path )
    {
        ArchiveType = g_list_prepend ( ArchiveType, ".7z");
	    ArchiveSuffix = g_list_prepend ( ArchiveSuffix, "*.7z");
		g_free (absolute_path);
    }
}

gboolean SpawnSyncCommand (gchar *command)
{
    GError *error = NULL;
    gchar *std_out;
    gchar *std_err;
	gchar **argv;
	int argcp;

	g_shell_parse_argv ( command , &argcp , &argv , NULL);
	if ( ! g_spawn_sync (
		NULL,
		argv,
		NULL,
		G_SPAWN_SEARCH_PATH,
		NULL,
		NULL, //user data
		&std_out,
		&std_err,
		&exit_status,
		&error) )
	{
		response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, "Can't spawn the command:",error->message);
		g_error_free (error);
		g_strfreev ( argv );
        return FALSE;
	}
    if ( WIFEXITED (exit_status) )
	{
	    if ( WEXITSTATUS (exit_status) )
		{
			response = ShowGtkMessageDialog (NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"An error occurred!",std_err );
			return FALSE;
		}
	}
	g_strfreev ( argv );
    return TRUE;
}

#ifdef GNM_USE_HILDON
gint dbus_req_handler( const gchar *interface, const gchar *method,
			GArray *arguments, gpointer data, osso_rpc_t *retval ) {
	//TODO
}

void cb_openUrls (void *app, int argc, gchar **argv) {  
	//TODO
}
#endif

int main (int argc, char **argv)
{
#ifndef GNM_USE_HILDON	
	gtk_init (&argc, &argv);
#else
	hildon_gtk_init (&argc, &argv);
	smode = 0;
#endif
	g_get_charset (&locale);

	addFolder = FALSE;
	extractSelected = FALSE;
	whereToExtract = g_strdup(confLoadCurrentLocation());

	g_printf("\n\n\nWAZNE: %s\n\n\n", whereToExtract);

	if ( cli_error != NULL )
	{
		g_print ("eaman: %s\nTry eaman --help to see a full list of available command line options.\n",cli_error->message);
		g_error_free (cli_error);
		return 0;
	}
	if (ask_and_extract || ask_and_add || archive_name != NULL || extract_path != NULL)
		cli = TRUE;

	if (cli == TRUE)
	{
		GetAvailableCompressors();
		ArchiveSuffix = g_list_reverse (ArchiveSuffix);
		ArchiveType = g_list_reverse (ArchiveType);
		MainWindow = gui_create_main_window ();
		gtk_main_iteration_do (FALSE);
		g_print ("Easy Archive Manager (\xC2\xA9)2010 Tomasz Pieniążek\n\n");

		/* Switch -x */
		if (extract_path != NULL)
		{
			if (argv[1] == NULL)
			{
				response = ShowGtkMessageDialog (NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't extract files from the archive:","You missed the archive name!\n");
				return 0;
			}
			for ( x = 1; x < argc; x++)
			{
				archive_cmd = am_a_init_structure_from_cmd_line ( argv[x] );
				if (archive_cmd != NULL)
				{
					GString *string = g_string_new ( "" );
					gchar *escaped_path = EscapeBadChars (extract_path , "$\'`\"\\!?* ()[]&|@#:;");
					archive_cmd->extraction_path = g_strdup (extract_path);
					cli_command = am_a_extract_single_files ( archive_cmd , string, escaped_path );
					g_free (escaped_path);
					if ( cli_command != NULL )
						error_output = SpawnSyncCommand ( cli_command );
					g_string_free (string, TRUE);
				}
			}
		}

		/* Switch -e */
		if (ask_and_extract)
		{
			if (argv[1] == NULL)
			{
				response = ShowGtkMessageDialog (NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't extract files from the archive:","You missed the archive name!\n");
				return 0;
			}
			archive_cmd = am_a_init_structure_from_cmd_line ( argv[1] );
			if (archive_cmd != NULL)
			{
				extract_window = am_a_create_extract_dialog ( 0 , archive_cmd);
				cli_command = am_a_parse_extract_dialog_options ( archive_cmd , extract_window, NULL );
				gtk_widget_destroy ( extract_window->dialog1 );
				if ( cli_command != NULL )
					error_output = SpawnSyncCommand ( cli_command );
				g_free (extract_window);
			}
		}
		/* Switch -d */
		else if (archive_name != NULL)
		{
			AM_Archive *archive_cmd = NULL;
			archive_cmd = am_a_new_archive_dialog (archive_name );
			if (archive_cmd == NULL)
				return 0;

			if (archive_cmd->path != NULL)
			{
				_current_dir = g_path_get_dirname(archive_name);
				chdir (_current_dir);
				g_free (_current_dir);
				GString *string = g_string_new ( "" );

				if ( g_file_test ( archive_name,G_FILE_TEST_EXISTS) )
				{
					_current_dir = g_path_get_basename ( archive_name );
					ConcatenateFileNames2 ( _current_dir, string );
					g_free (_current_dir);
				}

				for (x = 1; x< argc; x++)
				{
					_current_dir = g_path_get_basename ( argv[x] );
					ConcatenateFileNames2 ( _current_dir, string );
					g_free (_current_dir);
				}
				cli_command = am_a_add_single_files ( archive_cmd , string);
				if (cli_command != NULL)
					error_output = SpawnSyncCommand ( cli_command );
				g_string_free (string, TRUE);
			}
			if (cli_command != NULL)
				g_free (cli_command);
		}
		/* Switch -a */
		else if (ask_and_add)
		{
			if (argv[1] == NULL)
			{
				response = ShowGtkMessageDialog (NULL,GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't add files to the archive:","You missed the archive name!\n");
				return 0;
			}
			archive_cmd = am_a_init_structure_from_cmd_line ( argv[1] );
			if (archive_cmd != NULL)
			{
				add_window = am_a_create_add_dialog (archive_cmd);
				cli_command = am_a_parse_add_dialog_options ( archive_cmd, add_window );
				gtk_widget_destroy ( add_window->dialog1 );
				if (cli_command != NULL)
					error_output = SpawnSyncCommand ( cli_command );
				g_free (add_window);
			}
		}
	g_list_free ( ArchiveSuffix);
		g_list_free ( ArchiveType);
		if (archive_cmd != NULL)
			am_a_clean_archive_structure ( archive_cmd );
		return exit_status;
	}
	else
	{

	GetAvailableCompressors();
	ArchiveSuffix = g_list_reverse (ArchiveSuffix);
	ArchiveType = g_list_reverse (ArchiveType);

	MainWindow = gui_create_main_window ();
	am_a_show_cmd_line_output (NULL);
	gtk_window_set_position (GTK_WINDOW (MainWindow),GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW(MainWindow), 840, 440);
	gtk_widget_show (MainWindow);

#ifdef GNM_USE_HILDON
	hildon_program = HILDON_PROGRAM (hildon_program_get_instance ());
	hildon_program_add_window (hildon_program, MainWindow);

	osso_initialize ("com.nokia.eaman", APP_VERSION, TRUE, NULL);
	osso_mime_set_cb (osso_context, cb_openUrls, NULL);

	hildon_gtk_window_set_portrait_flags (MainWindow,HILDON_PORTRAIT_MODE_SUPPORT);
/*
    	if (osso_rpc_set_cb_f (osso_context,
                "com.nokia.archmanager",
                "/com/nokia/archmanager",
                "com.nokia.archmanager",
                dbus_req_handler, NULL) != OSSO_OK) {
        	hildon_banner_show_information (MainWindow,
        	       	NULL, "Failed to set dbus_req_handler");
    	}
*/
#endif

	/* This to open the archive_cmd from the command line */
	if ( argc == 2 )
	{
		gchar *dummy = g_strdup(argv[1]);
		current_open_directory = g_path_get_dirname (dummy);
		am_a_open_archive ( NULL , dummy );
	}
	
	gtk_main ();

	g_list_free ( ArchiveSuffix);
	g_list_free ( ArchiveType);

	return 0;
	}
}
