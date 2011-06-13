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
 * Creates "Extract" dialog.
 * Contains all needed functions to extract files from archive.
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include "extract.h"
#include "gui.h"
#include "gui_cb.h"

gboolean stop_flag;
extern gboolean cli;
extern gboolean unrar;
gchar *rar;

/**
 * Creates Extract window
 *
 * WARNING: Depracted Function!
 *
 * @param selected Number of the selected files
 * @param archive Should contain AM_Archive type of data
 *
 */
Extract_dialog_data *am_a_create_extract_dialog (gint selected , AM_Archive *archive)
{
	Extract_dialog_data *dialog_data;
	stop_flag = FALSE;

	dialog_data = g_new0 (Extract_dialog_data, 1);
	dialog_data->dialog1 = gtk_dialog_new ();
	gtk_window_set_title (GTK_WINDOW (dialog_data->dialog1), "Extract files");
	gtk_window_set_type_hint (GTK_WINDOW (dialog_data->dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_transient_for ( GTK_WINDOW (dialog_data->dialog1) , GTK_WINDOW (MainWindow) );

	dialog_data->dialog_vbox1 = GTK_DIALOG (dialog_data->dialog1)->vbox;
	gtk_widget_show (dialog_data->dialog_vbox1);

	dialog_data->vbox1 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (dialog_data->vbox1);
	gtk_box_pack_start (GTK_BOX (dialog_data->dialog_vbox1), dialog_data->vbox1, TRUE, TRUE, 0);

#ifdef GNM_USE_HILDON
	dialog_data->hbox3 = gtk_vbox_new (FALSE, 0);
#else
	dialog_data->hbox3 = gtk_hbox_new (FALSE, 0);
#endif
	gtk_widget_show (dialog_data->hbox3);
	gtk_box_pack_start (GTK_BOX (dialog_data->vbox1), dialog_data->hbox3, FALSE, TRUE, 17);

	dialog_data->extract_to_label = gtk_label_new ("Extract to:");
	gtk_widget_show (dialog_data->extract_to_label);
	gtk_box_pack_start (GTK_BOX (dialog_data->hbox3), dialog_data->extract_to_label, FALSE, TRUE, 0);

	dialog_data->destination_path_entry = gtk_entry_new ();
	gtk_widget_show (dialog_data->destination_path_entry);
	gtk_box_pack_start (GTK_BOX (dialog_data->hbox3), dialog_data->destination_path_entry, TRUE, TRUE, 0);
	gtk_widget_set_size_request (dialog_data->destination_path_entry, 385, -1);
	gtk_entry_set_activates_default (GTK_ENTRY (dialog_data->destination_path_entry), TRUE);

	gchar *dummy = g_strrstr (archive->path, ".");
	if (dummy != NULL)
	{
		dummy++;
		unsigned short int x = strlen (archive->path) - strlen ( dummy );
		gchar *extraction_string = (gchar *) g_malloc ( x + 1);
		strncpy ( extraction_string, archive->path, x );
		extraction_string [x-1] = '\0';

		if ( strstr (extraction_string , ".tar") )
		{
			extraction_string = g_realloc ( extraction_string, x - 5);
			strncpy ( extraction_string, archive->path, x - 5);
			extraction_string[x-5] = '\0';
		}
		gtk_entry_set_text (GTK_ENTRY(dialog_data->destination_path_entry), extraction_string);
		g_free (extraction_string);
	}
	else
		gtk_entry_set_text (GTK_ENTRY(dialog_data->destination_path_entry), archive->path);

#ifdef GNM_USE_HILDON
	dialog_data->button1 = hildon_button_new (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT,
                                HILDON_BUTTON_ARRANGEMENT_VERTICAL);
	hildon_button_set_title(dialog_data->button1, "Change folder");
#else
	dialog_data->button1 = gtk_button_new ();
	gtk_widget_set_size_request (dialog_data->button1, 33, 27);
	dialog_data->image1 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (dialog_data->image1);
	gtk_widget_set_size_request (dialog_data->image1, 30, 30);

	gtk_container_add(GTK_CONTAINER(dialog_data->button1), dialog_data->image1);
#endif

	gtk_box_pack_end(GTK_BOX (dialog_data->hbox3), dialog_data->button1, FALSE, TRUE, 0);
	gtk_widget_show (dialog_data->button1);

	g_signal_connect ( (gpointer) dialog_data->button1, "clicked", G_CALLBACK (am_a_choose_extraction_directory) , dialog_data );

#ifdef GNM_USE_HILDON
	dialog_data->hbox4 = gtk_vbox_new (TRUE, 7);
#else
	dialog_data->hbox4 = gtk_hbox_new (TRUE, 7);
#endif
	gtk_widget_show (dialog_data->hbox4);
	gtk_box_pack_start (GTK_BOX (dialog_data->vbox1), dialog_data->hbox4, FALSE, FALSE, 0);

#ifdef GNM_USE_HILDON
dialog_data->extract_button = hildon_button_new (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT,
                                HILDON_BUTTON_ARRANGEMENT_VERTICAL);
#else
	dialog_data->extract_button = gtk_button_new();
#endif
	dialog_data->extract_hbox = gtk_hbox_new(FALSE, 4);
	dialog_data->extract_label = gtk_label_new_with_mnemonic("_Extract");

	dialog_data->alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (dialog_data->alignment3);
	gtk_container_add (GTK_CONTAINER (dialog_data->alignment3), dialog_data->extract_hbox);

	gtk_box_pack_start(GTK_BOX(dialog_data->extract_hbox), dialog_data->extract_label, FALSE, FALSE, 0);
	gtk_widget_show_all(dialog_data->extract_hbox);
	gtk_container_add(GTK_CONTAINER(dialog_data->extract_button), dialog_data->alignment3);
	gtk_widget_show (dialog_data->extract_button);
	gtk_dialog_add_action_widget (GTK_DIALOG (dialog_data->dialog1), dialog_data->extract_button, GTK_RESPONSE_OK);
	GTK_WIDGET_SET_FLAGS (dialog_data->extract_button, GTK_CAN_DEFAULT);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog_data->dialog1), GTK_RESPONSE_OK);
	return dialog_data;
}

/**
 * Parses options and prepares to extract archive
 *
 * @param archive Should contain AM_Archive type of data
 * @param dialog_data Should contain Extract_dialog_data type of data
 * @param selection Our selected files
 *
 * @return A command to execute
 *
 */
gchar *am_a_parse_extract_dialog_options ( AM_Archive *archive , Extract_dialog_data *dialog_data, GtkTreeSelection *selection)
{
	gchar *command = NULL;
	gchar *tar;
	gchar *destination_path = NULL;
	gboolean done = FALSE;
	gboolean end = FALSE;
	GtkTreeIter iter;
	GString *names;

	gtk_widget_set_sensitive (Open_button,FALSE);

	if (unrar)
		rar = "unrar";
	else
		rar = "rar";

    while ( ! done )
	{
		switch ( gtk_dialog_run ( GTK_DIALOG (dialog_data->dialog1 ) ) )
		{
			case GTK_RESPONSE_CANCEL:
			case GTK_RESPONSE_DELETE_EVENT:
			done = TRUE;
			if (archive->type == AM_ARCHIVETYPE_GZIP || archive->type == AM_ARCHIVETYPE_BZIP2)
			{
				gtk_widget_set_sensitive (Stop_button,FALSE);
				gtk_widget_hide (viewport2);
				archive->status = AM_ARCHIVESTATUS_IDLE;
			}
			break;

			case GTK_RESPONSE_OK:
			destination_path = g_strdup (gtk_entry_get_text ( GTK_ENTRY (dialog_data->destination_path_entry) ));
			if (archive->type != AM_ARCHIVETYPE_ISO)
				archive->extraction_path = EscapeBadChars ( destination_path , "$\'`\"\\!?* ()&|@#:;" );
			else
				archive->extraction_path = g_strdup ( destination_path );

			if ( strlen ( archive->extraction_path ) == 0 )
			{
				response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, "You missed where to extract the files!","Please enter the extraction path." );
				break;
			}
			if ( g_file_test (destination_path , G_FILE_TEST_EXISTS) == FALSE)
			{
				int result = mkdir (destination_path , S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXGRP);
				if ( result == -1 )
				{
					gchar *msg = g_strdup_printf("Can't create directory \"%s\"", destination_path);
					response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, msg, g_strerror(errno ) );
					g_free ( msg);
					break;
				}
			}
			if ( g_file_test (destination_path , G_FILE_TEST_IS_DIR) && access (destination_path, R_OK | W_OK | X_OK ) )
			{
				gchar *utf8_path;
				gchar  *msg;

                utf8_path = g_filename_to_utf8 (destination_path, -1, NULL, NULL, NULL);
                msg = g_strdup_printf ("You don't have the right permissions to extract the files to the folder \"%s\".", utf8_path);
				response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, "Can't perform extraction!",msg );
                g_free (utf8_path);
				g_free (msg);
				g_free (destination_path);
				break;
			}
			done = TRUE;

			gtk_widget_set_sensitive (Stop_button,TRUE);
			gtk_widget_hide (dialog_data->dialog1);
			archive->status = AM_ARCHIVESTATUS_EXTRACT;

			if(!extractSelected){
				if ( ! cli )
				{
					gchar *text = g_strdup_printf("Extracting files to %s", destination_path);
					g_free (text);
				}
				g_free (destination_path);
        			tar = g_find_program_in_path ("gtar");
        			if (tar == NULL)
          				tar = g_strdup ("tar");

				switch ( archive->type )
				{
					case AM_ARCHIVETYPE_BZIP2:
						gzip_bzip2_extract (archive , 0);
					break;

					case AM_ARCHIVETYPE_GZIP:
						gzip_bzip2_extract (archive , 1);
					break;

					case AM_ARCHIVETYPE_RAR:
						command = g_strconcat ( rar, " " , "x ",
												"-u ",
												" -p",FALSE,
												" -o+",
												" -idp ",
												archive->escaped_path , " " , archive->extraction_path , NULL );
					break;

					case AM_ARCHIVETYPE_TAR:
						command = g_strconcat (tar, " -xvf ", archive->escaped_path,
											" --overwrite",
											" -C " , archive->extraction_path , NULL );
					break;

					case AM_ARCHIVETYPE_TAR_BZ2:
						command = g_strconcat (tar, " -xvjf " , archive->escaped_path,
											" --overwrite",
											" -C " , archive->extraction_path , NULL );
					break;

					case AM_ARCHIVETYPE_TAR_GZ:
						command = g_strconcat (tar, " -xvzf " , archive->escaped_path,
											" --overwrite",
											" -C " , archive->extraction_path , NULL );
					break;

					case AM_ARCHIVETYPE_DEB:
						command = g_strconcat (tar, " -xvzf " , archive->tmp,
											" --overwrite",
											" -C " , archive->extraction_path , NULL );
					break;

                    			case AM_ARCHIVETYPE_ZIP:
						command = g_strconcat ( "unzip ",
												"-u ",
												"-o",
												" -P " , FALSE,
												"",
												archive->escaped_path , " -d ", archive->extraction_path , NULL );
					break;

					case AM_ARCHIVETYPE_RPM:
						chdir ( archive->extraction_path );
						command = g_strconcat ( "cpio --make-directories -F " , archive->tmp , " -i" , NULL );
                    			break;


					default:
					command = NULL;
				}
				g_free (tar);

				if ( command != NULL )
					return command;
			}
			else
			{
				names = g_string_new ( " " );
				gtk_tree_selection_selected_foreach (selection, (GtkTreeSelectionForeachFunc) ConcatenateFileNames, names );
				command = am_a_extract_single_files ( archive , names, archive->extraction_path );
				g_string_free (names, TRUE);
				extractSelected = FALSE;
			}
		}
	}
	return command;
}

/**
 * Internal function, used by the others to delete temporary directory
 *
 * @param dir_name Name of the directory, we want to remove
 * @param flag If TRUE - watch child process
 *
 * @return Returns a result of the operation
 */
gboolean am_a_delete_temp_directory ( gchar *dir_name, gboolean flag)
{
	gchar *command;
	gboolean result;

	command = g_strconcat ( "rm -rf ", dir_name , NULL );
	result = am_a_run_command (command , flag );
	g_free (command);
	return result;
}

gboolean am_a_create_temp_directory ( gchar tmp_dir[] )
{
#ifdef GNM_NUSE_TMP
	if(strlen(whereToExtract) > 1023)
		strcpy (tmp_dir,"/tmp/xa-XXXXXX");
	else {
		gchar *tmp = g_strconcat(whereToExtract, "/tmp/xa-XXXXXX", NULL);
		strcpy (tmp_dir, tmp);
	}
		g_printf("\n\nTMP_DIR: %s\n\n", tmp_dir);
#else
	strcpy (tmp_dir,"/tmp/xa-XXXXXX");
#endif

	if ( mkdtemp ( tmp_dir ) == 0)
	{
		response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't create temporary directory: ",g_strerror(errno) );
		gtk_widget_set_sensitive (Stop_button, FALSE);
		return FALSE;
	}
	return TRUE;
}

/**
 * Creates GtkFileDialog and lets us to choose a place to extract data
 *
 * @param widget Our parent
 * @param data Should contain Extract_dialog_data type
 *
 */
void am_a_choose_extraction_directory (GtkWidget *widget, gpointer data)
{
	Extract_dialog_data *dialog_data = data;
	GtkWidget *File_Selector;
	int response;
	gchar *path;

#ifdef GNM_USE_HILDON
	File_Selector = GTK_FILE_CHOOSER (hildon_file_chooser_dialog_new (MainWindow, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER));
#else
	File_Selector = gtk_file_chooser_dialog_new ( "Choose the destination folder where to extract the archive",
					GTK_WINDOW (MainWindow),
					GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
					GTK_STOCK_CANCEL,
					GTK_RESPONSE_CANCEL,
					GTK_STOCK_OPEN,
					GTK_RESPONSE_ACCEPT,
					NULL );
#endif
	response = gtk_dialog_run (GTK_DIALOG (File_Selector));
	if ((response == GTK_RESPONSE_ACCEPT) || (response == GTK_RESPONSE_OK))
	{
		path = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER (File_Selector) );
		gtk_entry_set_text (GTK_ENTRY(dialog_data->destination_path_entry),path);
		g_free (path);
	}
	gtk_widget_destroy (File_Selector);
}

/**
 * Internal function, used by the others to extract a file (single)
 *
 * @param archive Our archive, on which we will be operating
 * @param flag files List of files we want to extract
 * @param path Extraction path
 *
 * @return Returns a command to execute
 */
gchar *am_a_extract_single_files ( AM_Archive *archive , GString *files, gchar *path)
{
	gchar *command = NULL;
	gchar *tar;
	GtkTreeIter iter;

	if (unrar)
		rar = "unrar";
	else
		rar = "rar";

	gchar *msg = g_strdup_printf ( "Extracting archive to %s" , path);

	tar = g_find_program_in_path ("gtar");

	if (tar == NULL)
		tar = g_strdup ("tar");

	switch (archive->type)
	{
		case AM_ARCHIVETYPE_BZIP2:
			gzip_bzip2_extract (archive , 0);
			break;

		case AM_ARCHIVETYPE_GZIP:
			gzip_bzip2_extract (archive , 1);
			break;

		case AM_ARCHIVETYPE_RAR:
			command = g_strconcat ( rar," ", "x ", "", "-u ", "-o+",
									" -idp ",
									archive->escaped_path , " " , files->str , " ", path ,NULL);
			break;

		case AM_ARCHIVETYPE_TAR:
			command = g_strconcat (tar, " -xvf " , archive->escaped_path,
								" --overwrite",
								"",
								" -C " , path , files->str , NULL );

			break;

		case AM_ARCHIVETYPE_TAR_BZ2:
			command = g_strconcat (tar, " -xjvf " , archive->escaped_path,
								" --overwrite",
								"",
								" -C " , path , files->str , NULL );
			break;

		case AM_ARCHIVETYPE_TAR_GZ:
			command = g_strconcat (tar, " -xzvf " , archive->escaped_path,
								" --overwrite",
								"",
								" -C " , path , files->str , NULL );
			break;

		case AM_ARCHIVETYPE_DEB:
			command = g_strconcat (tar, " -xvzf " , archive->tmp,
					" --overwrite",
					"",
					" -C " , archive->extraction_path , files->str, NULL );
			break;

		case AM_ARCHIVETYPE_ZIP:
			command = g_strconcat ( "unzip ",
									"-u ",
									"-o ",
									"",
									archive->escaped_path , files->str," -d " , path , NULL );
			break;

        	case AM_ARCHIVETYPE_RPM:
			chdir ( path );
			command = g_strconcat ( "cpio --make-directories " , files->str , " -F " , archive->tmp , " -i" , NULL);
        		break;

        	case AM_ARCHIVETYPE_7ZIP:
			command = g_strconcat ( "7za " , "x",
									" -aoa",
									" -bd ",
									archive->escaped_path , files->str , " -o" , path , NULL );
        	break;

		case AM_ARCHIVETYPE_ISO:
		{
			GList *row_list = NULL;
			GtkTreeSelection *selection;

			selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW (archive->treeview) );
			row_list = gtk_tree_selection_get_selected_rows (selection, &archive->model);
			while (row_list)
			{
				if (stop_flag)
					break;
				gtk_tree_model_get_iter(archive->model, &iter, row_list->data);
				gtk_tree_model_get (archive->model, &iter,
				0, &name,
				1, &permissions,
				2, &file_size,
				4, &file_offset,
				-1);
				gtk_tree_path_free (row_list->data);

				am_a_extract_single_iso_file (archive, permissions, archive->extraction_path, name , file_size, file_offset );
				g_free (name);
				g_free (permissions);
				row_list = row_list->next;
			}
			g_list_free (row_list);
			command = NULL;
		}
		break;

		default:
		command = NULL;
    }
	g_free (tar);
	return command;
}

