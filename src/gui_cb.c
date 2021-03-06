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
 * Callback functions, mainly for MainWindow.
 * It's a main layer between MainWindow and dialogs, such as "Add file".
 */

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "gui.h"
#include "gui_cb.h"
#include "main.h"
#include "utils.h"
#include "config.h"

#ifdef GNM_USE_HILDON
#include <hildon/hildon-file-chooser-dialog.h>
#endif

extern gboolean cli;
GList *Suffix;
GtkFileFilter *open_file_filter = NULL;
gchar *current_open_directory = NULL;
GList *Name;
extern gboolean unrar;
extern gboolean stop_flag;

/**
 * Internal function, used by the others to watch a child process
 *
 * @param pid Process pid
 * @param status Archive status
 * @param data Should contain AM_Archive type of data
 *
 * @return Returns a result of the operation
 */
void am_a_watch_child ( GPid pid, gint status, gpointer data)
{
	AM_Archive *archive = data;
	gboolean new	= FALSE;
	gboolean open	= FALSE;
	gboolean add	= FALSE;
	gboolean extract= FALSE;
	gboolean select	= FALSE;
	gboolean check	= FALSE;
	gboolean info	= FALSE;
	gboolean waiting = TRUE;
	int ps;


	indicateProcessingData(TRUE);
	gtk_widget_set_sensitive (close1,TRUE);

	if ( archive->type == AM_ARCHIVETYPE_BZIP2 || archive->type == AM_ARCHIVETYPE_GZIP )
	{
		new = open = TRUE;
		info = FALSE;
	}
	else if (archive->type == AM_ARCHIVETYPE_RPM || archive->type == AM_ARCHIVETYPE_DEB)
	{
		new = open = extract = select = info = TRUE;
	}
	else if (archive->type == AM_ARCHIVETYPE_TAR_BZ2 || archive->type == AM_ARCHIVETYPE_TAR_GZ || archive->type == AM_ARCHIVETYPE_TAR )
	{
		new = open = add = extract = select = info = TRUE;
		check = FALSE;
	}
	else if (archive->type == AM_ARCHIVETYPE_RAR && unrar)
	{
		check = TRUE;
		add = FALSE;
		new = open = extract = select = info = TRUE;
	}
	else
	{
		check = TRUE;
		new = open = add = extract = select = info = TRUE;
	}
	if ( WIFSIGNALED (status) )
	{
		if (archive->status == AM_ARCHIVESTATUS_EXTRACT)
		{
			gchar *msg = g_strdup_printf("Some files could have been extracted",archive->extraction_path);

            		response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_INFO,	GTK_BUTTONS_OK,"",msg );
            		g_free (msg);
		}
		indicateProcessingData(FALSE);
		return;
	}

	if ( WIFEXITED (status) )
	{
		if ( WEXITSTATUS (status) )
		{
			response = ShowGtkMessageDialog (GTK_WINDOW	(MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Error","Show the command line output?" );
			if (response == GTK_RESPONSE_YES)
				am_a_show_cmd_line_output (NULL);

			indicateProcessingData(FALSE);
			return;
		}
	}

	if ( ! cli )
	{
		/* Automatically reload the content of the archive after adding or deleting */
		if (archive->status == AM_ARCHIVESTATUS_DELETE || archive->status == AM_ARCHIVESTATUS_ADD)
		{
			switch (archive->type)
			{
				case AM_ARCHIVETYPE_RAR:
			    		OpenRar(archive);
				break;

				case AM_ARCHIVETYPE_TAR:
					OpenTar(archive);
				break;

				case AM_ARCHIVETYPE_TAR_BZ2:
					OpenBzip2(archive);
				break;

				case AM_ARCHIVETYPE_TAR_GZ:
					OpenGzip(archive);
				break;

				case AM_ARCHIVETYPE_ZIP:
					OpenZip(archive);
				break;

				case AM_ARCHIVETYPE_7ZIP:
					Open7Zip(archive);
				break;

				default:
				break;
			}
			archive->status = AM_ARCHIVESTATUS_IDLE;
			while (waiting)
			{
				ps = waitpid ( archive->child_pid, &status, WNOHANG);
				if (ps < 0)
					waiting = FALSE;
				else
					gtk_main_iteration_do (FALSE);
			}
		}
	}

	if (! cli && archive != NULL)
	{
		gtk_widget_show ( pad_image );
	}
	gtk_widget_grab_focus (GTK_WIDGET(archive->treeview));
	indicateProcessingData(FALSE);
}

/**
 * Creates a new archive
 *
 * @param menuitem Parents menuitem
 * @param user_data Data we want to pass
 *
 */
void am_a_new_archive (GtkMenuItem *menuitem, gpointer user_data)
{
//	indicateProcessingData(TRUE);
	gint current_page;

	current_page = am_a_get_new_archive_idx();
	if (current_page == -1)
		return;

	archive[current_page] = am_a_new_archive_dialog ( NULL );

	if (archive[current_page]  == NULL)
		return;

	am_a_add_page (archive[current_page]);

    	EmptyTextBuffer();
    	gtk_widget_set_sensitive ( view_shell_output1 , TRUE );
    
    	gtk_widget_hide ( pad_image );

    	archive[current_page]->dummy_size = 0;
    	archive[current_page]->nr_of_files = 0;
    	archive[current_page]->nr_of_dirs = 0;
	am_a_set_window_title (MainWindow , archive[current_page]->path );
	gtk_widget_set_sensitive ( close1 , TRUE );
	gtk_widget_set_sensitive ( AddFile_button , TRUE );
	gtk_widget_set_sensitive ( Extract_button , TRUE );
	gtk_widget_set_sensitive ( Delete_button , TRUE );
#ifdef GNM_USE_HILDON
	gtk_widget_hide(openB);
	gtk_widget_hide(newB);
	gtk_widget_show(closeArchiveB);
#endif
	indicateProcessingData(FALSE);
}

/**
 * Close currently active archive
 *
 * @param menuitem Parents menuitem
 * @param user_data List of files (from MainWindow)
 *
 */
void am_a_close_archive (GtkMenuItem *menuitem, gpointer user_data)
{
	gint current_page;
	gint idx;
	GtkWidget *scrollwindow = user_data;
	
	indicateProcessingData(TRUE);

	gtk_widget_set_sensitive (new1, TRUE);	
	gtk_widget_set_sensitive (open1, TRUE);	
	gtk_widget_set_sensitive (Open_button, TRUE);	
	gtk_widget_set_sensitive (New_button, TRUE);	

	gtk_widget_set_sensitive (AddFile_button, FALSE);
	gtk_widget_set_sensitive (Extract_button, FALSE);
	gtk_widget_set_sensitive (Delete_button, FALSE);
	gtk_widget_set_sensitive (addfile, FALSE);
	gtk_widget_set_sensitive (extract_menu, FALSE);
	gtk_widget_set_sensitive (delete_menu, FALSE);
	gtk_widget_set_sensitive (close1,FALSE);
	gtk_widget_set_sensitive (select_all,FALSE);
	gtk_widget_set_sensitive (deselect_all,FALSE);

	current_page = gtk_notebook_page_num(notebook, scrollwindow);
	idx = am_a_find_archive_index (current_page);
	gtk_notebook_remove_page ( notebook , current_page);

	current_page = gtk_notebook_get_n_pages(notebook);
	if ( current_page == 0)
	{
		//gtk_widget_set_sensitive (close1,FALSE);
		am_a_set_window_title (MainWindow,NULL);
	}
	else if ( current_page == 1)
		gtk_notebook_set_show_tabs (notebook,FALSE);
	else
		gtk_notebook_set_show_tabs (notebook,TRUE);

	EmptyTextBuffer();
	am_a_clean_archive_structure (archive[idx]);
#ifdef GNM_USE_HILDON
	gtk_widget_set_sensitive ( openB , TRUE );
#endif
#ifdef GNM_USE_HILDON
	gtk_widget_show(openB);
	gtk_widget_show(newB);
	gtk_widget_hide(closeArchiveB);
#endif
	indicateProcessingData(FALSE);
}

/**
 * Select all elements from the list
 *
 * @param menuitem Parents menuitem
 * @param user_data Data we want to pass
 *
 */
void am_a_select_all ( GtkMenuItem *menuitem , gpointer user_data )
{
	gint idx;
	gint current_page;

	current_page = gtk_notebook_get_current_page (notebook);
	idx = am_a_find_archive_index (current_page);

	current_page = gtk_notebook_get_current_page (notebook);
	gtk_tree_selection_select_all ( gtk_tree_view_get_selection (GTK_TREE_VIEW (archive[idx]->treeview) ) );
	gtk_widget_set_sensitive (select_all,FALSE);
	gtk_widget_set_sensitive (deselect_all,TRUE);
}

/**
 * Deselect all elements from the list
 *
 * @param menuitem Parents menuitem
 * @param user_data Data we want to pass
 *
 */
void am_a_deselect_all ( GtkMenuItem *menuitem , gpointer user_data )
{
	gint current_page;
	gint idx;

	current_page = gtk_notebook_get_current_page (notebook);
	idx = am_a_find_archive_index (current_page);

	gtk_tree_selection_unselect_all ( gtk_tree_view_get_selection (GTK_TREE_VIEW (archive[idx]->treeview) ) );
	gtk_widget_set_sensitive (select_all,TRUE);
	gtk_widget_set_sensitive (deselect_all,FALSE);
}

/**
 * Create a new message dialog
 *
 * @param window Parents window
 * @param mode Flag passed directly to the gtk_message_dialog_new
 * @param type Type of message
 * @param button Set of buttons to use
 * @param meesage1 Title of the dialog
 * @param message2 Our message
 *
 * @return Returns response
 */
int ShowGtkMessageDialog ( GtkWindow *window, int mode, int type, int button, const gchar *message1, const gchar *message2)
{
	dialog = gtk_message_dialog_new (window, mode, type, button,message1);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), message2);
	response = gtk_dialog_run (GTK_DIALOG (dialog) );
	gtk_widget_destroy (GTK_WIDGET (dialog) );
	return response;
}

/**
 * Open an archive
 *
 * @param window Parents window
 * @param user_data Data we want to pass
 *
 */
void am_a_open_archive (GtkMenuItem *menuitem, gpointer data)
{
	indicateProcessingData(TRUE);
	g_printf("\nam_a_open_archive\n");
	gchar *path = NULL;
	gint current_page;
	gint x;
	path = (gchar *)data;

	if ( path == NULL)
	{
		path = am_a_open_file_dialog ();
		if (path == NULL)
			return;
	}

	current_page = am_a_get_new_archive_idx();
	if (current_page == -1)
		return;

	archive[current_page] = am_a_init_archive_structure();
	if (archive[current_page] == NULL)
	{
		response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't allocate memory for the archive structure:","Operation aborted!");
		g_free (path);
		return;
	}
	g_printf("\nam_a_open_archive: PATH: %s\n", path);
	archive[current_page]->path = g_strdup (path);
	archive[current_page]->escaped_path = EscapeBadChars ( archive[current_page]->path , "$\'`\"\\!?* ()&|@#:;" );

	am_a_add_page (archive[current_page]);
	archive[current_page]->type = am_a_detect_archive_type ( archive[current_page] , NULL );
	if ( archive[current_page]->type == -2 )
	{
		am_a_close_archive ( NULL, NULL);
		return;
	}
	if ( archive[current_page]->type == -1 )
	{
		am_a_close_archive ( NULL, NULL);
		gchar *utf8_path,*msg;
		utf8_path = g_filename_to_utf8 (path, -1, NULL, NULL, NULL);
		msg = g_strdup_printf ("Can't open file \"%s\":", utf8_path);
		am_a_set_window_title (MainWindow , NULL);
		response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,msg, "Archive format is not recognized!");
		g_free (utf8_path);
		g_free (msg);
		g_free (path);
		indicateProcessingData(FALSE);
		return;
	}
	g_free (path);
	EmptyTextBuffer();
	gchar *ext = NULL;
	if ( archive[current_page]->type == AM_ARCHIVETYPE_RAR )
		ext = ".rar";
	else if ( archive[current_page]->type == AM_ARCHIVETYPE_7ZIP )
		ext = ".7z";

	if ( ext != NULL )
		if ( ! g_list_find ( ArchiveType , ext ) )
		{
			response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,"RAR is not supported:","necessary tool is not installed!" );
			indicateProcessingData(FALSE);
			return;
		}

	archive[current_page]->status = AM_ARCHIVESTATUS_OPEN;
	switch ( archive[current_page]->type )
	{

		case AM_ARCHIVETYPE_BZIP2:
			OpenBzip2 (archive[current_page]);
		break;

		case AM_ARCHIVETYPE_GZIP:
			OpenGzip ( archive[current_page] );
		break;

		case AM_ARCHIVETYPE_RAR:
			OpenRar (archive[current_page]);
		break;

		case AM_ARCHIVETYPE_TAR:
			OpenTar (archive[current_page]);
		break;

		case AM_ARCHIVETYPE_ZIP:
			OpenZip (archive[current_page]);
		break;

		case AM_ARCHIVETYPE_DEB:
			OpenDeb (archive[current_page]);
		break;

		case AM_ARCHIVETYPE_ISO:
			OpenISO (archive[current_page]);
		break;

		case AM_ARCHIVETYPE_7ZIP:
			Open7Zip (archive[current_page]);
		break;

		case AM_ARCHIVETYPE_RPM:
			OpenRPM (archive[current_page]);
		break;

		default:
		break;
	}
#ifdef GNM_USE_HILDON
	gtk_widget_hide(openB);
	gtk_widget_hide(newB);
	gtk_widget_show(closeArchiveB);
#endif

	gtk_widget_set_sensitive (New_button, FALSE);
	gtk_widget_set_sensitive (Open_button, FALSE);
	gtk_widget_set_sensitive (new1, FALSE);
	gtk_widget_set_sensitive (open1, FALSE);

	gtk_widget_set_sensitive (AddFile_button, TRUE);
	gtk_widget_set_sensitive (Extract_button, TRUE);
	gtk_widget_set_sensitive (Delete_button, TRUE);

	gtk_widget_set_sensitive (addfile, TRUE);
	gtk_widget_set_sensitive (extract_menu, TRUE);
	gtk_widget_set_sensitive (delete_menu, TRUE);
	gtk_widget_set_sensitive (close1,TRUE);
	gtk_widget_set_sensitive (select_all,TRUE);
	gtk_widget_set_sensitive (deselect_all,FALSE);

	//indicateProcessingData(FALSE);
}

/**
 * Stop current task
 *
 * @param window Parents window
 * @param user_data Data we want to pass
 *
 */
void am_a_cancel_archive ( GtkMenuItem *menuitem , gpointer data )
{
	gint current_page;
	gint idx;

	current_page = gtk_notebook_get_current_page(notebook);
	idx = am_a_find_archive_index (current_page);

	if (archive[idx]->status == AM_ARCHIVESTATUS_ADD)
	{
		response = ShowGtkMessageDialog (GTK_WINDOW(MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Data under process!","Do you really want to cancel?" );
		if (response == GTK_RESPONSE_NO)
			return;
	}

	stop_flag = TRUE;
	if (archive[idx]->type != AM_ARCHIVETYPE_ISO)
	{
		if ( kill ( archive[idx]->child_pid , SIGABRT ) < 0 )
	    {
		    response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"An error occurred while trying to kill the process:",g_strerror(errno));
			return;
	    }
	}
}

/**
 * Remove (selected) elements from an archive
 *
 * @param window Parents window
 * @param user_data Data we want to pass
 *
 */
void am_a_delete_archive (GtkMenuItem *menuitem, gpointer user_data)
{
	gchar *command = NULL;
	gchar *tar;
	gint x;
	GString *names;
	gint current_page;
	gint id;

	indicateProcessingData(TRUE);

	current_page = gtk_notebook_get_current_page ( notebook);
	id = am_a_find_archive_index ( current_page);

	GtkTreeSelection *selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW (archive[id]->treeview) );
	names = g_string_new ( " " );
	gtk_tree_selection_selected_foreach (selection, (GtkTreeSelectionForeachFunc) ConcatenateFileNames, names );

	x = gtk_tree_selection_count_selected_rows (selection);
	gchar *msg = g_strdup_printf ("Trying to delete %d file(s) from the archive.",x);
	response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,msg, "Are you sure?" );
	g_free (msg);

	if ( response == GTK_RESPONSE_NO){
		indicateProcessingData(FALSE);
		return;
	}

	archive[id]->status = AM_ARCHIVESTATUS_DELETE;

	tar = g_find_program_in_path ("gtar");
	if (tar == NULL)
		tar = g_strdup ("tar");

	switch (archive[id]->type)
	{
	case AM_ARCHIVETYPE_RAR:
		command = g_strconcat ( "rar d " , archive[id]->escaped_path , names->str , NULL );
		break;

        case AM_ARCHIVETYPE_TAR:
		command = g_strconcat (tar, " --delete -vf " , archive[id]->escaped_path , names->str , NULL );
		break;

        case AM_ARCHIVETYPE_TAR_BZ2:
        	am_add_delete_tar_bzip2_gzip ( names , archive[id] , 0 , 0 );
        break;

        case AM_ARCHIVETYPE_TAR_GZ:
        	am_add_delete_tar_bzip2_gzip ( names , archive[id] , 1 , 0 );
		break;

        case AM_ARCHIVETYPE_ZIP:
		command = g_strconcat ( "zip -d " , archive[id]->escaped_path , names->str , NULL );
		break;

        default:
        break;
	}

	if (command != NULL) {
	    	gtk_widget_set_sensitive (Stop_button,TRUE);
        	am_a_run_command ( command , 1);
	        g_free (command);
	}

	g_string_free (names , TRUE );
	g_free (tar);
	indicateProcessingData(FALSE);
}

/**
 * Add files to an archive
 *
 * @param window Parents window
 * @param user_data Data we want to pass
 *
 */
void am_a_add_files_archive (GtkMenuItem *menuitem, gpointer data )
{
	gchar *command = NULL;
	gint current_page;
	gint idx;

	indicateProcessingData(TRUE);

	current_page = gtk_notebook_get_current_page (notebook);
	idx = am_a_find_archive_index ( current_page );


	command = am_a_select_files_to_add (archive[idx]);

	if (command != NULL)
	{
		am_a_run_command (command , 1);
		g_free (command);
	}

	indicateProcessingData(FALSE);
}

void am_a_cat_filenames_basename (GtkTreeModel *model, GtkTreePath *treepath, GtkTreeIter *iter, GString *data)
{
	gchar *fullname;
	gchar *name;

	gtk_tree_model_get (model, iter, 1, &fullname, -1);
	name = g_path_get_basename ( fullname );

	ConcatenateFileNames2 ( name , data );
	g_free (fullname);
}

void am_a_cat_filenames (GtkTreeModel *model, GtkTreePath *treepath, GtkTreeIter *iter, GString *data)
{
	gchar *fullname;

	gtk_tree_model_get (model, iter, 1, &fullname, -1);
	ConcatenateFileNames2 ( fullname , data );
	g_free (fullname);
}

/**
 * Extract an archive
 *
 * @param window Parents window
 * @param user_data Data we want to pass
 *
 */
void am_a_extract_archive ( GtkMenuItem *menuitem , gpointer user_data )
{
	gchar *command = NULL;
	gint current_page;
	gint idx;

	indicateProcessingData(TRUE);

	current_page = gtk_notebook_get_current_page (notebook);
	idx = am_a_find_archive_index (current_page);

	GtkTreeSelection *selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW (archive[idx]->treeview) );
	gint selected = gtk_tree_selection_count_selected_rows ( selection );
    	extract_window = am_a_create_extract_dialog (selected , archive[idx]);

	if (archive[idx]->extraction_path != NULL)
		gtk_entry_set_text (GTK_ENTRY(extract_window->destination_path_entry),archive[idx]->extraction_path);

    	command = am_a_parse_extract_dialog_options ( archive[idx] , extract_window, selection );

	if (extract_window->dialog1 != NULL)
	{
		gtk_widget_destroy ( extract_window->dialog1 );
		extract_window->dialog1 = NULL;
	}

	if (command != NULL)
	{
		gtk_widget_set_sensitive ( close1 , 	FALSE);
		am_a_run_command (command , 1);
		g_free (command);
	}

	g_free (extract_window);
	extract_window = NULL;
	indicateProcessingData(FALSE);
}

/**
 * Create Open file dialog
 *
 */
gchar *am_a_open_file_dialog ()
{
	static GtkWidget *File_Selector = NULL;
	GtkFileFilter *filter;
	gchar *path = NULL;

	if (File_Selector == NULL)
	{
#ifdef GNM_USE_HILDON
		File_Selector = GTK_FILE_CHOOSER (hildon_file_chooser_dialog_new (MainWindow, GTK_FILE_CHOOSER_ACTION_OPEN));
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(File_Selector), "/home/user/MyDocs/.documents");

#else
		File_Selector = gtk_file_chooser_dialog_new ( "Open",
						GTK_WINDOW (MainWindow),
						GTK_FILE_CHOOSER_ACTION_OPEN,
						GTK_STOCK_CANCEL,
						GTK_RESPONSE_CANCEL,
						"gtk-open",
						GTK_RESPONSE_ACCEPT,
						NULL);
#endif

		gtk_dialog_set_default_response (GTK_DIALOG (File_Selector), GTK_RESPONSE_ACCEPT);
		gtk_window_set_destroy_with_parent (GTK_WINDOW (File_Selector) , TRUE);

		filter = gtk_file_filter_new ();
		gtk_file_filter_set_name ( filter , "All files" );
		gtk_file_filter_add_pattern ( filter, "*" );
		gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (File_Selector), filter);


		filter = gtk_file_filter_new ();
		gtk_file_filter_set_name ( filter , "Only archives" );
		Suffix = g_list_first ( ArchiveSuffix );
		while ( Suffix != NULL )
		{
			gtk_file_filter_add_pattern (filter, Suffix->data);
			Suffix = g_list_next ( Suffix );
		}
		gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (File_Selector), filter);

		Suffix = g_list_first ( ArchiveSuffix );
		while ( Suffix != NULL )
		{
			if ( Suffix->data != "" )
			{
				filter = gtk_file_filter_new ();
				gtk_file_filter_set_name (filter, Suffix->data );
				gtk_file_filter_add_pattern (filter, Suffix->data );
				gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (File_Selector), filter);
			}
			Suffix = g_list_next ( Suffix );
		}
		gtk_window_set_modal (GTK_WINDOW (File_Selector),TRUE);
	}
	if (open_file_filter != NULL)
		gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (File_Selector) , open_file_filter );

	if (current_open_directory != NULL)
		gtk_file_chooser_set_current_folder ( GTK_FILE_CHOOSER (File_Selector) , current_open_directory );

	response = gtk_dialog_run (GTK_DIALOG (File_Selector));

	if (current_open_directory != NULL)
		g_free (current_open_directory);

	current_open_directory = gtk_file_chooser_get_current_folder ( GTK_FILE_CHOOSER (File_Selector) );
	open_file_filter = gtk_file_chooser_get_filter ( GTK_FILE_CHOOSER (File_Selector) );
#ifndef GNM_USE_HILDON
	if (response == GTK_RESPONSE_ACCEPT)
#else
	if (response == GTK_RESPONSE_OK)
#endif
		path = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER (File_Selector) );
	else if ( (response == GTK_RESPONSE_CANCEL) || (response == GTK_RESPONSE_DELETE_EVENT) )
		path = NULL;

	gtk_widget_hide (File_Selector);
	return path;
}

/**
 * Create a list which is used to display list of files
 *
 * @param window Parents window
 * @param column_names[] Defines the columns names
 * @param column_types[] Defines the columns types
 * @param archive Should contain AM_Archive type of data
 *
 */
void am_a_create_liststore ( unsigned short int nc, gchar *columns_names[] , GType columns_types[], AM_Archive *archive)
{
	indicateProcessingData(TRUE);
	unsigned short int x;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	archive->liststore = gtk_list_store_newv ( nc , (GType *)columns_types);
	gtk_tree_view_set_model ( GTK_TREE_VIEW (archive->treeview), GTK_TREE_MODEL (archive->liststore) );

	archive->model = gtk_tree_view_get_model(GTK_TREE_VIEW(archive->treeview));
	g_object_ref(archive->model);
	gtk_tree_view_set_model(GTK_TREE_VIEW(archive->treeview), NULL);

	for (x = 0; x <= nc-1; x++)
	{
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes ( columns_names[x],renderer,"text",x,NULL);
		
		if(columns_names[x] == "Filename"){
			gtk_tree_view_column_set_visible (column, TRUE);
		}
		else
			gtk_tree_view_column_set_visible (column, FALSE);

		gtk_tree_view_column_set_resizable (column, TRUE);
		gtk_tree_view_column_set_sort_column_id (column, x);
		
		/** 
		** To avoid bug with inserting existing columns
		**/
		if(!archive->ticker){
			gtk_tree_view_append_column (GTK_TREE_VIEW (archive->treeview), column);
			archive->ticker = TRUE;
		}
	}
	indicateProcessingData(FALSE);
}

/**
 * Detect archive type
 *
 * @param archive Should contain AM_Archive type of data
 * @param filename Name of the archive
 *
 * @return Returns archive type
 */
int am_a_detect_archive_type ( AM_Archive *archive , gchar *filename )
{
	FILE *dummy_ptr = NULL;
    	int xx = -1;
	unsigned char magic[12];

	if (filename != NULL)
		dummy_ptr = fopen ( filename , "r" );
	else
		dummy_ptr = fopen ( archive->path , "r" );

	if (dummy_ptr == NULL)
	{
		if ( !cli )
		{
			gchar *utf8_path,*msg;
			utf8_path = g_filename_to_utf8 (archive->path, -1, NULL, NULL, NULL);
			msg = g_strdup_printf ("Can't open archive \"%s\":" , utf8_path );
			response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow) , GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,
			msg,g_strerror (errno));
			g_free (msg);
			g_free (utf8_path);
			return -2;
		}
		else
			return -2;
	}
	if ( fread ( magic, 1, 12, dummy_ptr ) == 0 )
	{
		fclose ( dummy_ptr);
		return -2;
	}

	if ( memcmp ( magic,"\x50\x4b\x03\x04",4 ) == 0 || memcmp ( magic,"\x50\x4b\x05\x06",4 ) == 0 )
	{
		xx = AM_ARCHIVETYPE_ZIP;
	}
	else if ( memcmp ( magic,"\x52\x61\x72\x21",4 ) == 0 )
	{
		 xx = AM_ARCHIVETYPE_RAR;
	}
	else if ( memcmp ( magic,"\x42\x5a\x68",3 ) == 0 ) xx = AM_ARCHIVETYPE_BZIP2;
	else if ( memcmp ( magic,"\x1f\x8b",2) == 0 || memcmp ( magic,"\x1f\x9d",2 ) == 0 )  xx = AM_ARCHIVETYPE_GZIP;
	else if ( memcmp ( magic,"\xed\xab\xee\xdb",4 ) == 0) xx = AM_ARCHIVETYPE_RPM;
	else if ( memcmp ( magic,"\x37\x7a\xbc\xaf\x27\x1c",6 ) == 0 ) xx = AM_ARCHIVETYPE_7ZIP;
	else if ( isTar ( dummy_ptr ) ) xx = AM_ARCHIVETYPE_TAR;
	else if ( isISO ( dummy_ptr ) ) xx = AM_ARCHIVETYPE_ISO;
	else if ( memcmp ( magic,"!<arch>\n", 8 ) == 0) xx = AM_ARCHIVETYPE_DEB;

	fclose ( dummy_ptr );
	return xx;
}

/**
 * Creates a new cmd line output window
 *
 * @param menuitem Parents window
 *
 */
void am_a_show_cmd_line_output( GtkMenuItem *menuitem )
{
	if (OutputWindow != NULL)
	{
		gtk_window_set_title (GTK_WINDOW (OutputWindow), "Output" );
		gtk_window_present ( GTK_WINDOW (OutputWindow) );
		return;
	}
#ifndef GNM_USE_HILDON
	OutputWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
#else
	OutputWindow = hildon_stackable_window_new ();
#endif
	gtk_window_set_position (GTK_WINDOW (OutputWindow), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(OutputWindow), 320, 380);
	gtk_window_set_destroy_with_parent (GTK_WINDOW (OutputWindow), TRUE);
	g_signal_connect (G_OBJECT (OutputWindow), "delete-event",  G_CALLBACK (gtk_widget_hide), &OutputWindow);

	vbox = gtk_vbox_new ( FALSE, 2 );
	scrollwin = gtk_scrolled_window_new ( NULL,NULL );
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW( scrollwin ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	textview = gtk_text_view_new();
	gtk_text_view_set_editable (GTK_TEXT_VIEW(textview), FALSE);
	gtk_container_add (GTK_CONTAINER(scrollwin), textview);
	gtk_box_pack_start (GTK_BOX(vbox), scrollwin, TRUE, TRUE, 0);
	gtk_container_add (GTK_CONTAINER(OutputWindow), vbox);
	textbuf = gtk_text_view_get_buffer ( GTK_TEXT_VIEW(textview) );
	gtk_text_buffer_get_start_iter (textbuf, &enditer);

#ifdef GNM_USE_HILDON
	if(smode == 0)
		hildon_gtk_window_set_portrait_flags (GTK_WINDOW (OutputWindow),HILDON_PORTRAIT_MODE_SUPPORT);
	else if(smode == 1)
		hildon_gtk_window_set_portrait_flags (GTK_WINDOW (OutputWindow),HILDON_PORTRAIT_MODE_REQUEST);
#endif

	gtk_widget_show (vbox);
	gtk_widget_show (scrollwin);
	gtk_widget_show (textview);
}

/**
 * Executes a command
 *
 * @param command Command we want to execute
 * @param watch_child_flag TRUE if we want to watch child process
 *
 * @return TRUE if succeed, otherwise FALSE
 */
gboolean am_a_run_command ( gchar *command , gboolean watch_child_flag )
{
	gint current_page;
	gint idx;
	int status;
	gboolean waiting = TRUE;
	int ps;

	current_page = gtk_notebook_get_current_page (notebook);
	idx = am_a_find_archive_index ( current_page );

	archive[idx]->parse_output = 0;
	SpawnAsyncProcess ( archive[idx] , command , 0, 1);

	if ( archive[idx]->child_pid == 0 )
		return FALSE;

//	gtk_widget_show (viewport2);

	while (waiting)
	{
		ps = waitpid ( archive[idx]->child_pid, &status, WNOHANG);
		if (ps < 0)
			waiting = FALSE;
		else
			gtk_main_iteration_do (FALSE);
	}
	if (watch_child_flag)
	{
		am_a_watch_child (archive[idx]->child_pid, status, archive[idx]);
		return TRUE;
	}
	else
	{
		if ( WIFEXITED (status) )
		{
			if ( WEXITSTATUS (status) )
			{
				gtk_widget_hide ( pad_image );
//				gtk_widget_hide ( viewport2 );
				am_a_set_window_title (MainWindow , NULL);
				response = ShowGtkMessageDialog (GTK_WINDOW(MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"An error occurred while accessing the archive.","Show the command line output?" );
				if (response == GTK_RESPONSE_YES)
					am_a_show_cmd_line_output (NULL);
				archive[idx]->status = AM_ARCHIVESTATUS_IDLE;
				gtk_widget_set_sensitive (Stop_button,FALSE);
				return FALSE;
			}
		}
	}
	return TRUE;
}

/**
 * Used to close application
 *
 * @param menuitem Parents menuitem
 * @param user_data Data we want to pass
 *
 */
void gui_cb_application_quit(GtkMenuItem *menuitem, gpointer user_data)
{
	gint i;
	gint idx;

	g_list_free(Suffix);
	g_list_free(Name);

	for (i = 0; i < gtk_notebook_get_n_pages(notebook); i++)
	{
		idx = am_a_find_archive_index (i);
		if (archive[idx] != NULL)
			am_a_clean_archive_structure (archive[idx]);
	}

	if (current_open_directory != NULL)
		g_free (current_open_directory);

	gtk_main_quit();
}

gboolean treeview_select_search (GtkTreeModel *model,gint column,const gchar *key,GtkTreeIter *iter,gpointer search_data)
{
    	char *filename;
	gboolean result;

	gtk_tree_model_get (model, iter, 0, &filename, -1);

	if ( strcasestr (filename, key) ) 
		result = FALSE;
        else result = TRUE;

	g_free (filename);

	return result;
}

gboolean isTar ( FILE *ptr )
{
	unsigned char magic[7];

	fseek (ptr, 0 , SEEK_SET);

    	if (fseek ( ptr , 257 , SEEK_CUR) < 0 )
		return FALSE;
	if (fread ( magic, 1, 7, ptr) == 0 )
		return FALSE;
    	if (memcmp ( magic,"\x75\x73\x74\x61\x72\x00\x30",7) == 0 || memcmp (magic,"\x75\x73\x74\x61\x72\x20\x20",7) == 0)
		return TRUE;
    	else
		return FALSE;
}

void ConcatenateFileNames2 (gchar *filename , GString *data)
{
	gchar *esc_filename = NULL;
	gchar *escaped = NULL;
	gchar *escaped2 = NULL;
	gint current_page;
	gint idx;

	current_page = gtk_notebook_get_current_page (notebook);
	idx = am_a_find_archive_index (current_page);

	if ( strstr (filename, "[") || strstr (filename, "]"))
	{
		if (archive[idx]->type == AM_ARCHIVETYPE_ZIP)
		{
			if (archive[idx]->status == AM_ARCHIVESTATUS_ADD)
			{
				esc_filename = EscapeBadChars ( filename ,"$\'`\"\\!?* ()[]&|@#:;" );
				g_string_prepend (data, esc_filename);
			}
			else
			{
				escaped = EscapeBadChars ( filename ,"$\'`\"\\!?* ()[]&|@#:;");
				escaped2 = escape_str_common (escaped , "*?[]", '\\', 0);
				g_free (escaped);
				esc_filename = escaped2;
				g_string_prepend (data, esc_filename);
			}
		}
		else if ( archive[idx]->type == AM_ARCHIVETYPE_TAR_BZ2 || archive[idx]->type == AM_ARCHIVETYPE_TAR_GZ || archive[idx]->type == AM_ARCHIVETYPE_TAR )
		{
			if (archive[idx]->status == AM_ARCHIVESTATUS_ADD)
			{
				esc_filename = EscapeBadChars ( filename ,"$\'`\"\\!?* ()[]&|@#:;" );
				g_string_prepend (data, esc_filename);
			}
			else
			{
				escaped = EscapeBadChars ( filename ,"?*\\'& !|()@#:;");
				escaped2 = escape_str_common ( escaped , "[]", '[', ']');
				g_free (escaped);
				esc_filename = escaped2;
				g_string_prepend (data, esc_filename);
			}
		}
	}
	else
	{
		esc_filename = EscapeBadChars ( filename , "$\'`\"\\!?* ()[]&|@#:;" );
		g_string_prepend (data, esc_filename);
	}
	g_string_prepend_c (data, ' ');
	g_free (esc_filename);
}

void ConcatenateFileNames (GtkTreeModel *model, GtkTreePath *treepath, GtkTreeIter *iter, GString *data)
{
	gchar *filename = NULL;

	gtk_tree_model_get (model, iter, 0, &filename, -1);
	ConcatenateFileNames2 ( filename , data );
	g_free (filename);
}

void EmptyTextBuffer()
{
	if (textbuf != NULL)
	{
		gtk_text_buffer_get_start_iter (textbuf,&start);
		gtk_text_buffer_get_end_iter (textbuf,&end);
		gtk_text_buffer_delete (textbuf,&start,&end);
		gtk_text_buffer_get_start_iter(textbuf, &enditer);
	}
}

/**
 * Creates a new "About" dialog
 *
 * @param window Parents window
 * @param user_data Data we want to pass
 *
 */
void am_a_about (GtkMenuItem *menuitem, gpointer user_data)
{
    static GtkWidget *about = NULL;
    const char *authors[] = {"\nAuthor: Tomasz Pieniążek <t.pieniazek@gazeta.pl>",NULL};

	if (about == NULL)
	{
		about = gtk_about_dialog_new ();
		gtk_window_set_destroy_with_parent (GTK_WINDOW (about) , TRUE);
		g_object_set (about,
				"name",  "Easy Archive Manager",
				"version", APP_VERSION,
				"authors", authors,
				"logo_icon_name", "filemanager_debian_file",
				"website", "https://garage.maemo.org/projects/eaman",
				"license", 
		      			"This is free software; you can redistribute it and/or\n"
    					"modify it under the terms of the GNU Library General Public License as\n"
    					"published by the Free Software Foundation; either version 2 of the\n"
    					"License, or (at your option) any later version.\n"
    					"\n"
    					"This software is distributed in the hope that it will be useful,\n"
    					"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    					"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
    					"Library General Public License for more details.\n"
    					"\n"
    					"You should have received a copy of the GNU Library General Public\n"
    					"License along with the Gnome Library; see the file COPYING.LIB.  If not,\n"
    					"write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,\n"
    					"Boston, MA 02111-1307, USA.\n",
		      NULL);
		gtk_window_set_position (GTK_WINDOW (about), GTK_WIN_POS_CENTER);
	}
	gtk_dialog_run (GTK_DIALOG(about));
	gtk_widget_hide (about);
}

/**
 * Function cb for adding a whole folder to archive
 *
 */
void addFolderM_cb (){
	addFolder = TRUE;
	am_a_add_files_archive (NULL, NULL);
}

/**
 * Function cb for adding a file to archive
 *
 */
void addFileM_cb (){
	addFolder = FALSE;
	am_a_add_files_archive (NULL, NULL);
}

/**
 * Function cb for extracting all elements from archive
 *
 */
void extractEverythingM_cb (){
	extractSelected = FALSE;
	am_a_extract_archive (NULL, NULL);
}

/**
 * Function cb for extracting selected elements from archive
 *
 */
void extractSelectedM_cb (){
	extractSelected = TRUE;
	am_a_extract_archive (NULL, NULL);
	am_a_deselect_all (NULL, NULL);
}

/**
 * Indicate processing data.
 *
 * As for now, it's Hildon-only
 *
 * @param processing If you want to indicate set TRUE, otherwise FALSE
 */
void indicateProcessingData (gboolean processing) {
	if(processing){
#ifdef GNM_USE_HILDON
		hildon_gtk_window_set_progress_indicator(MainWindow, 1);
#endif
		gtk_widget_set_sensitive(Stop_button, TRUE);
		gtk_widget_set_sensitive(AddFile_button, FALSE);
		gtk_widget_set_sensitive(Extract_button, FALSE);
		gtk_widget_set_sensitive(Delete_button, FALSE);
	}	
	else {
#ifdef GNM_USE_HILDON
		hildon_gtk_window_set_progress_indicator(MainWindow, 0);
#endif
		gtk_widget_set_sensitive(Stop_button, FALSE);
		gtk_widget_set_sensitive(AddFile_button, TRUE);
		gtk_widget_set_sensitive(Extract_button, TRUE);
		gtk_widget_set_sensitive(Delete_button, TRUE);
	}
}

/**
 * Checks id FILE is an ISO image
 *
 * @param ptr, file we want to check
 */
gboolean isISO ( FILE *ptr )
{
	int offset_image;
	if ((offset_image = DetectImage(ptr)) > 0 )
	{
		fseek ( ptr , offset_image, SEEK_SET );
		fread ( &ipd, 1, sizeof(ipd), ptr );
		system_id = g_strndup ( ipd.system_id, 30);
		volume_id = g_strndup ( ipd.volume_id, 30);
		application_id = g_strndup ( ipd.application_id, 126);
		publisher_id = g_strndup ( ipd.publisher_id, 126);
		preparer_id = g_strndup ( ipd.preparer_id, 126);

		creation_date = g_strdup_printf ("%4.4s %2.2s %2.2s %2.2s:%2.2s:%2.2s.%2.2s",&ipd.creation_date[0],&ipd.creation_date[4],&ipd.creation_date[6],&ipd.creation_date[8],&ipd.creation_date[10],&ipd.creation_date[12],&ipd.creation_date[14]);
		modified_date = g_strdup_printf ("%4.4s %2.2s %2.2s %2.2s:%2.2s:%2.2s.%2.2s",&ipd.modification_date[0],&ipd.modification_date[4],&ipd.modification_date[6],&ipd.modification_date[8],&ipd.modification_date[10],&ipd.modification_date[12],&ipd.modification_date[14]);
		expiration_date = g_strdup_printf ("%4.4s %2.2s %2.2s %2.2s:%2.2s:%2.2s.%2.2s",&ipd.expiration_date[0],&ipd.expiration_date[4],&ipd.expiration_date[6],&ipd.expiration_date[8],&ipd.expiration_date[10],&ipd.expiration_date[12],&ipd.expiration_date[14]);
		effective_date = g_strdup_printf ("%4.4s %2.2s %2.2s %2.2s:%2.2s:%2.2s.%2.2s",&ipd.effective_date[0],&ipd.effective_date[4],&ipd.effective_date[6],&ipd.effective_date[8],&ipd.effective_date[10],&ipd.effective_date[12],&ipd.effective_date[14]);

        	return TRUE;
	}
    	else
		return FALSE;
}

gboolean am_a_report_child_stderr (GIOChannel *ioc, GIOCondition cond, gpointer data)
{
	GIOStatus status;
	gchar     buffer[4096];
	gsize     bytes_read;

	if (cond & (G_IO_IN | G_IO_PRI))
	{
		do
		{
			status = g_io_channel_read_chars (ioc, buffer, sizeof (buffer), &bytes_read, NULL);
			if (bytes_read > 0)
				gtk_text_buffer_insert (textbuf, &enditer, buffer, bytes_read);
		}
		while (status == G_IO_STATUS_NORMAL);
		if (status == G_IO_STATUS_ERROR || status == G_IO_STATUS_EOF)
			goto done;
	}
	else if (cond & (G_IO_ERR | G_IO_HUP | G_IO_NVAL) )
	{
		done:
			g_io_channel_shutdown (ioc, TRUE, NULL);
			g_io_channel_unref (ioc);
			return FALSE;
	}
	return TRUE;
}
