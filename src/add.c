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
 * Creates "Add files" and "New archive" dialogs.
 * Contains all neded functions to add files to an archive.
 */

#include <gtk/gtk.h>
#include "add.h"
#include "gui.h"
#include "gui_cb.h"

extern gboolean unrar;
extern gboolean cli;

gint new_combo_box = -1;
gchar *current_new_directory = NULL;

static const GtkTargetEntry drop_targets[] =
{
	{ "text/uri-list", 0, 0 },
};

/**
 * Function used to opening zip archives.
 *
 * WARNING: Function depracted!
 *
 * @param archive Archive we want to open
 *
 */
Add_dialog_data *am_a_create_add_dialog (AM_Archive *archive)
{
Add_dialog_data *add_dialog;
	unsigned short int default_value, max_value;
	default_value = max_value = 0;
	gchar *compression_msg = NULL;

	add_dialog = g_new0 (Add_dialog_data, 1);

	add_dialog->dialog1 = gtk_dialog_new ();
	gtk_window_set_title (GTK_WINDOW (add_dialog->dialog1), "Add files");
	gtk_window_set_type_hint (GTK_WINDOW (add_dialog->dialog1), GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_transient_for ( GTK_WINDOW (add_dialog->dialog1) , GTK_WINDOW (MainWindow) );
	gtk_window_set_resizable (GTK_WINDOW (add_dialog->dialog1), FALSE);

	add_dialog->dialog_vbox1 = GTK_DIALOG (add_dialog->dialog1)->vbox;
	gtk_widget_show (add_dialog->dialog_vbox1);

	add_dialog->frame5 = gtk_frame_new (NULL);
	gtk_widget_show (add_dialog->frame5);
	gtk_box_pack_start (GTK_BOX (add_dialog->dialog_vbox1), add_dialog->frame5, TRUE, TRUE, 5);
	gtk_widget_set_size_request (add_dialog->frame5, 400, -1);

	add_dialog->alignment5 = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (add_dialog->alignment5);
	gtk_container_add (GTK_CONTAINER (add_dialog->frame5), add_dialog->alignment5);
	gtk_alignment_set_padding (GTK_ALIGNMENT (add_dialog->alignment5), 2, 5, 5, 5);

	add_dialog->vbox7 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (add_dialog->vbox7);
	gtk_container_add (GTK_CONTAINER (add_dialog->alignment5), add_dialog->vbox7);

	add_dialog->scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (add_dialog->scrolledwindow3);
	gtk_box_pack_start (GTK_BOX (add_dialog->vbox7), add_dialog->scrolledwindow3, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (add_dialog->scrolledwindow3), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (add_dialog->scrolledwindow3), GTK_SHADOW_IN);

	add_dialog->file_liststore = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);

//#ifdef GNM_USE_HILDON
//	add_dialog->file_list_treeview = hildon_gtk_tree_view_new_with_model(HILDON_UI_MODE_EDIT, GTK_TREE_MODEL(add_dialog->file_liststore));
//#else
	add_dialog->file_list_treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(add_dialog->file_liststore));
//#endif
	add_dialog->column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_visible (add_dialog->column, FALSE);

	add_dialog->selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(add_dialog->file_list_treeview) );
	gtk_tree_selection_set_mode(add_dialog->selection, GTK_SELECTION_MULTIPLE);

	add_dialog->renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start (add_dialog->column, add_dialog->renderer, FALSE);
	gtk_tree_view_column_add_attribute (add_dialog->column, add_dialog->renderer, "stock-id", 0);

	add_dialog->renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(add_dialog->column, add_dialog->renderer, TRUE);
	gtk_tree_view_column_add_attribute(add_dialog->column, add_dialog->renderer, "text", 1);

	gtk_tree_view_append_column(GTK_TREE_VIEW(add_dialog->file_list_treeview), add_dialog->column);

	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(add_dialog->file_list_treeview), TRUE);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (add_dialog->file_list_treeview), FALSE);
	gtk_container_add (GTK_CONTAINER (add_dialog->scrolledwindow3), add_dialog->file_list_treeview);
	gtk_widget_show (add_dialog->file_list_treeview);

	add_dialog->hbox1 = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (add_dialog->hbox1);
	gtk_box_pack_start (GTK_BOX (add_dialog->vbox7), add_dialog->hbox1, TRUE, TRUE, 0);


	add_dialog->hbuttonbox2 = gtk_hbutton_box_new ();
	gtk_widget_show (add_dialog->hbuttonbox2);
	gtk_box_pack_start (GTK_BOX (add_dialog->hbox1), add_dialog->hbuttonbox2, TRUE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (add_dialog->hbuttonbox2), 5);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (add_dialog->hbuttonbox2), GTK_BUTTONBOX_END);
	gtk_box_set_spacing (GTK_BOX (add_dialog->hbuttonbox2), 8);

	add_dialog->remove_button = gtk_button_new_from_stock ("gtk-remove");
	gtk_widget_set_sensitive ( add_dialog->remove_button, FALSE );
	gtk_widget_show (add_dialog->remove_button);
	gtk_container_add (GTK_CONTAINER (add_dialog->hbuttonbox2), add_dialog->remove_button);
	GTK_WIDGET_SET_FLAGS (add_dialog->remove_button, GTK_CAN_DEFAULT);
	g_signal_connect ( (gpointer) add_dialog->remove_button, "clicked", G_CALLBACK (remove_files_liststore) , add_dialog );

	add_dialog->add_files_button = gtk_button_new_from_stock ("gtk-add");
	gtk_widget_show (add_dialog->add_files_button);
	gtk_container_add (GTK_CONTAINER (add_dialog->hbuttonbox2), add_dialog->add_files_button);
	GTK_WIDGET_SET_FLAGS (add_dialog->add_files_button, GTK_CAN_DEFAULT);
	g_signal_connect ( (gpointer) add_dialog->add_files_button, "clicked", G_CALLBACK (am_a_select_files_to_add) , add_dialog );

	add_dialog->cancel_button = gtk_button_new_from_stock ("gtk-cancel");
	gtk_widget_show (add_dialog->cancel_button);
	gtk_dialog_add_action_widget (GTK_DIALOG (add_dialog->dialog1), add_dialog->cancel_button, GTK_RESPONSE_CANCEL);
	GTK_WIDGET_SET_FLAGS (add_dialog->cancel_button, GTK_CAN_DEFAULT);

	add_dialog->add_button = gtk_button_new();
	add_dialog->add_hbox = gtk_hbox_new(FALSE, 4);
	add_dialog->add_label = gtk_label_new_with_mnemonic("Add");

	add_dialog->alignment6 = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (add_dialog->alignment6);
	gtk_container_add (GTK_CONTAINER (add_dialog->alignment6), add_dialog->add_hbox);

	gtk_box_pack_start(GTK_BOX(add_dialog->add_hbox), add_dialog->add_image, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(add_dialog->add_hbox), add_dialog->add_label, FALSE, TRUE, 0);
	gtk_widget_show_all (add_dialog->add_hbox);
	gtk_container_add(GTK_CONTAINER(add_dialog->add_button), add_dialog->alignment6);
	gtk_widget_show (add_dialog->add_button);
	gtk_dialog_add_action_widget (GTK_DIALOG (add_dialog->dialog1), add_dialog->add_button, GTK_RESPONSE_OK);
	GTK_WIDGET_SET_FLAGS (add_dialog->add_button, GTK_CAN_DEFAULT);
	gtk_dialog_set_default_response (GTK_DIALOG (add_dialog->dialog1), GTK_RESPONSE_OK);
	return add_dialog;
}

void activate_remove_button (GtkTreeModel *tree_model, GtkTreePath *path, GtkTreeIter *iter, Add_dialog_data *data)
{
	if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(data->file_liststore), iter) == TRUE)
		gtk_widget_set_sensitive ( data->remove_button, TRUE );
}

/**
 * Internal function, prepare to add files.
 *
 * @param archive Archive we want to open
 *
 */
gchar* am_a_select_files_to_add ( AM_Archive *archive)
{
	GtkWidget *File_Selector;
	Add_dialog_data *add_dialog;
	GSList *list = NULL;
	gchar *title = NULL;
	GtkTreeIter iter;
	gchar *command = NULL;
	gchar *first_item = NULL;
	gboolean done = FALSE;
	GString *names;

	unsigned short int flag;

	add_dialog = am_a_create_add_dialog(archive);

	add_dialog->file_liststore = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);

//#ifdef GNM_USE_HILDON
//	add_dialog->file_list_treeview = hildon_gtk_tree_view_new_with_model(HILDON_UI_MODE_NORMAL, GTK_TREE_MODEL(add_dialog->file_liststore));
//#else
	add_dialog->file_list_treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(add_dialog->file_liststore));
//#endif
	add_dialog->column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_visible (add_dialog->column, FALSE);

#ifdef GNM_USE_HILDON
	if(addFolder)
		File_Selector = GTK_FILE_CHOOSER (hildon_file_chooser_dialog_new (MainWindow, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER));
	else
		File_Selector = GTK_FILE_CHOOSER (hildon_file_chooser_dialog_new (MainWindow, GTK_FILE_CHOOSER_ACTION_OPEN));
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(File_Selector), "/home/user/MyDocs/.documents");

#else
	title = "Select the files to add";
	if(addFolder)
		flag = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
	else
		flag = GTK_FILE_CHOOSER_ACTION_OPEN;

	File_Selector = gtk_file_chooser_dialog_new ( title,
							GTK_WINDOW (MainWindow),
							flag,
							GTK_STOCK_CANCEL,
							GTK_RESPONSE_CANCEL,
							GTK_STOCK_OPEN,
							GTK_RESPONSE_ACCEPT,
							NULL);
#endif
	addFolder = FALSE;
	gtk_file_chooser_set_select_multiple ( GTK_FILE_CHOOSER (File_Selector) , TRUE );


	response = gtk_dialog_run (GTK_DIALOG (File_Selector));
	if (((response == GTK_RESPONSE_ACCEPT) || (response == GTK_RESPONSE_OK)) && (response != -4))
	{
		list = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (File_Selector));
		g_slist_foreach( list, (GFunc) add_files_liststore, add_dialog->file_liststore);
			if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(add_dialog->file_liststore), &iter) == FALSE)
			{
				response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't add files to the archive:", "You haven't selected any files to add!" );
				return;
			}
			done = TRUE;

			gtk_tree_model_get_iter_first(GTK_TREE_MODEL(add_dialog->file_liststore), &iter);
			gtk_tree_model_get (GTK_TREE_MODEL(add_dialog->file_liststore), &iter, 1, &first_item, -1);
			gchar *current_dir = g_path_get_dirname ( first_item );
			g_free ( first_item );
			chdir ( current_dir );
			g_free ( current_dir );

			names = g_string_new ( " " );
			archive->status = AM_ARCHIVESTATUS_ADD;

			if ( archive->type == AM_ARCHIVETYPE_TAR || archive->type == AM_ARCHIVETYPE_TAR_BZ2 || archive->type == AM_ARCHIVETYPE_TAR_GZ ) 
			{
				while (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(add_dialog->file_liststore), &iter) )
				{
					am_a_cat_filenames_basename ( GTK_TREE_MODEL(add_dialog->file_liststore), NULL, &iter, names );
					gtk_list_store_remove (add_dialog->file_liststore, &iter);
				}
			}

			else
			{
				while (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(add_dialog->file_liststore), &iter) )
				{
					am_a_cat_filenames ( GTK_TREE_MODEL(add_dialog->file_liststore), NULL, &iter, names );
					gtk_list_store_remove (add_dialog->file_liststore, &iter);
				}
			}
			gtk_widget_set_sensitive ( Stop_button , TRUE);
#ifdef GNM_USE_HILDON
			hildon_gtk_window_set_progress_indicator(MainWindow, 1);
#endif
			gtk_widget_set_sensitive ( close1 , 	FALSE);
			gtk_widget_hide (add_dialog->dialog1);
			gtk_widget_hide (GTK_DIALOG (File_Selector));

			command = am_a_add_single_files ( archive, names);
			g_string_free ( names, TRUE);
	}

	if (list != NULL)
		g_slist_free (list);
	gtk_widget_destroy ( File_Selector );
	return command;
}

/**
 * Function used to add files to liststore
 *
 * @param file_path Files path
 * @param liststore Liststore to which we want add files
 *
 */
void add_files_liststore (gchar *file_path, GtkListStore *liststore)
{
	GtkTreeIter iter;
	gchar *icon = GTK_STOCK_FILE;
	gchar *file_utf8;

	file_utf8 = g_filename_display_name (file_path);
	if ( g_file_test(file_path, G_FILE_TEST_IS_DIR) == TRUE )
		icon = GTK_STOCK_DIRECTORY;

	gtk_list_store_append(liststore, &iter);
	gtk_list_store_set (liststore, &iter, 0, icon, 1, file_utf8, -1);
	g_free (file_utf8);
}

/**
 * Function used to remove files from liststore
 *
 * @param widget Parents widget
 * @param data Add_dialog_data type of data
 *
 */
void remove_files_liststore (GtkWidget *widget, gpointer data)
{
	Add_dialog_data *add_dialog = data;
	GtkTreeSelection *sel;
	GtkTreePath *path;
	GtkTreeIter iter;
	GList *rr_list = NULL;
	GList *node;

	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(add_dialog->file_list_treeview) );
	gtk_tree_selection_selected_foreach(sel, (GtkTreeSelectionForeachFunc) remove_foreach_func, &rr_list);

	for (node = rr_list; node != NULL; node = node->next)
	{
		path = gtk_tree_row_reference_get_path((GtkTreeRowReference *) node->data);
		if (path)
	    {
			if ( gtk_tree_model_get_iter(GTK_TREE_MODEL(add_dialog->file_liststore), &iter, path) )
				gtk_list_store_remove(add_dialog->file_liststore, &iter);
			gtk_tree_path_free(path);
		}
	}
	if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(add_dialog->file_liststore), &iter) == FALSE)
		gtk_widget_set_sensitive ( add_dialog->remove_button, FALSE);
	g_list_foreach(rr_list, (GFunc) gtk_tree_row_reference_free, NULL);
	g_list_free(rr_list);
}

void remove_foreach_func (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, GList **rowref_list)
{
	GtkTreeRowReference *rowref;

	rowref = gtk_tree_row_reference_new(model, path);
	*rowref_list = g_list_append(*rowref_list, rowref);
}

void fix_adjustment_value (GtkAdjustment *adjustment, gpointer user_data)
{
	unsigned short int digit = gtk_adjustment_get_value (adjustment);
	if (digit & 1)
		return;
	else
		gtk_adjustment_set_value (adjustment, digit-1);
}

/**
 * Prepare to add files to an archive.
 *
 * Parse options and prepare to add files to an archive
 *
 * @param archive Archive to which we want add files
 * @param add_dialog Parent Add_dialog_data
 *
 * @return Return command to execute
 */
gchar *am_a_parse_add_dialog_options ( AM_Archive *archive , Add_dialog_data *add_dialog )
{
	GtkTreeIter iter;
	gchar *command = NULL;
	gchar *first_item = NULL;
	gboolean done = FALSE;
	GString *names;

	while ( ! done )
	{
		switch (gtk_dialog_run ( GTK_DIALOG (add_dialog->dialog1 ) ) )
		{
			case GTK_RESPONSE_CANCEL:
			case GTK_RESPONSE_DELETE_EVENT:
			done = TRUE;
			break;

			case GTK_RESPONSE_OK:
			if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(add_dialog->file_liststore), &iter) == FALSE)
			{
				response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't add files to the archive:", "You haven't selected any files to add!" );
				break;
			}
			done = TRUE;

			gtk_tree_model_get_iter_first(GTK_TREE_MODEL(add_dialog->file_liststore), &iter);
			gtk_tree_model_get (GTK_TREE_MODEL(add_dialog->file_liststore), &iter, 1, &first_item, -1);
			gchar *current_dir = g_path_get_dirname ( first_item );
			g_free ( first_item );
			chdir ( current_dir );
			g_free ( current_dir );

			names = g_string_new ( " " );
			archive->status = AM_ARCHIVESTATUS_ADD;

			if ( archive->type == AM_ARCHIVETYPE_TAR || archive->type == AM_ARCHIVETYPE_TAR_BZ2 || archive->type == AM_ARCHIVETYPE_TAR_GZ ) 
			{
				while (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(add_dialog->file_liststore), &iter) )
				{
					am_a_cat_filenames_basename ( GTK_TREE_MODEL(add_dialog->file_liststore), NULL, &iter, names );
					gtk_list_store_remove (add_dialog->file_liststore, &iter);
				}
			}

			else
			{
				while (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(add_dialog->file_liststore), &iter) )
				{
					am_a_cat_filenames ( GTK_TREE_MODEL(add_dialog->file_liststore), NULL, &iter, names );
					gtk_list_store_remove (add_dialog->file_liststore, &iter);
				}
			}
			gtk_widget_set_sensitive ( Stop_button , TRUE);
			gtk_widget_set_sensitive ( close1 , 	FALSE);
			gtk_widget_hide (add_dialog->dialog1);

			command = am_a_add_single_files ( archive, names);
			g_string_free ( names, TRUE);
		}
	}
	return command;
}

/**
 * Adds single files to an archive
 *
 * @param archive Archive to which we want add files
 * @param names List of the files
 *
 * @return Return command to execute
 */
gchar *am_a_add_single_files ( AM_Archive *archive , GString *names)
{
	gchar *command = NULL;
	gchar *tar;

	tar = g_find_program_in_path ("gtar");
	if (tar == NULL)
		tar = g_strdup ("tar");

	switch (archive->type)
	{
		case AM_ARCHIVETYPE_RAR:
			command = g_strconcat ( "rar a ",
									" -r ",
									"-ep ",
									"-idp ",
									"-m"," ",
									archive->escaped_path,
									names->str , NULL );
		break;

		case AM_ARCHIVETYPE_TAR:
		if ( g_file_test ( archive->escaped_path , G_FILE_TEST_EXISTS ) )
			command = g_strconcat (tar, " ",
									"-rvvf ",
									archive->escaped_path,
									names->str , NULL );
		else
			command = g_strconcat (tar, " ",
									"-cvvf ",archive->escaped_path,
									names->str , NULL );
		break;

		case AM_ARCHIVETYPE_TAR_BZ2:
		if ( g_file_test ( archive->escaped_path , G_FILE_TEST_EXISTS ) )
			am_add_delete_tar_bzip2_gzip ( names , archive, 0 , 1 );
		else
			command = g_strconcat (tar, " ",
									"-cvvjf ",archive->escaped_path,
									names->str , NULL );
		break;

		case AM_ARCHIVETYPE_TAR_GZ:
		if ( g_file_test ( archive->escaped_path , G_FILE_TEST_EXISTS ) )
			am_add_delete_tar_bzip2_gzip ( names , archive, 1 , 1 );
		else
			command = g_strconcat (tar, " ",
									"-cvvzf ",archive->escaped_path,
									names->str , NULL );
		break;

		case AM_ARCHIVETYPE_ZIP:
			command = g_strconcat ( "zip ",
									"-r ",
									"-j ",
									"-"," ",
									archive->escaped_path,
									names->str , NULL );
		break;

		default:
		command = NULL;
	}
	g_free (tar);
	return command;
}

/**
 * Creates "New archive" dialog
 *
 * Parse options and prepare to add files to an archive
 *
 * @param path Path of the new archive
 *
 * @return Returns a new archive
 */
AM_Archive *am_a_new_archive_dialog (gchar *path)
{
	AM_Archive *archive = NULL;
	GtkWidget *am_a_file_chooser;
	GtkWidget *hbox = NULL;
	GtkWidget *combo_box = NULL;
	GtkWidget *add_extension_cb = NULL;
	GtkFileFilter *am_a_new_archive_dialog_filter;
	GList *Suffix,*Name;
	gchar *my_path = NULL;
	gchar *my_path_ext = NULL;
	gchar *basepath = NULL;
	gchar *current_dir = NULL;


#ifdef GNM_USE_HILDON
	am_a_file_chooser = GTK_FILE_CHOOSER (hildon_file_chooser_dialog_new (MainWindow, GTK_FILE_CHOOSER_ACTION_SAVE));
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(am_a_file_chooser), "/home/user/MyDocs/.documents");
#else
	am_a_file_chooser = gtk_file_chooser_dialog_new ( "Create a new archive",
							GTK_WINDOW (MainWindow),
							GTK_FILE_CHOOSER_ACTION_SAVE,
							GTK_STOCK_CANCEL,
							GTK_RESPONSE_CANCEL,
							"Create",
							GTK_RESPONSE_ACCEPT,
							NULL);
#endif

	gtk_dialog_set_default_response (GTK_DIALOG (am_a_file_chooser), GTK_RESPONSE_ACCEPT);

	am_a_new_archive_dialog_filter = gtk_file_filter_new ();
	gtk_file_filter_set_name ( am_a_new_archive_dialog_filter , "All files" );
	gtk_file_filter_add_pattern ( am_a_new_archive_dialog_filter, "*" );
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (am_a_file_chooser), am_a_new_archive_dialog_filter);

	am_a_new_archive_dialog_filter = gtk_file_filter_new ();
	gtk_file_filter_set_name ( am_a_new_archive_dialog_filter , "Only archives" );

	Suffix = g_list_first ( ArchiveSuffix );

	while ( Suffix != NULL )
	{
		gtk_file_filter_add_pattern (am_a_new_archive_dialog_filter, Suffix->data);
		Suffix = g_list_next ( Suffix );
	}
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (am_a_file_chooser), am_a_new_archive_dialog_filter);

	Suffix = g_list_first ( ArchiveSuffix );

	while ( Suffix != NULL )
	{
		if ( Suffix->data != "" )
		{
			am_a_new_archive_dialog_filter = gtk_file_filter_new ();
			gtk_file_filter_set_name (am_a_new_archive_dialog_filter, Suffix->data );
			gtk_file_filter_add_pattern (am_a_new_archive_dialog_filter, Suffix->data );
			gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (am_a_file_chooser), am_a_new_archive_dialog_filter);
		}
		Suffix = g_list_next ( Suffix );
	}
	hbox = gtk_vbox_new (FALSE, 12);
	gtk_box_pack_start (GTK_BOX (hbox),gtk_label_new ("Archive type:"),FALSE, FALSE, 0);

	combo_box = gtk_combo_box_new_text ();

	Name = g_list_first ( ArchiveType );
	while ( Name != NULL )
	{
		if (Name->data == ".tgz" || Name->data == ".rpm" || Name->data == ".iso" || Name->data == ".gz" || Name->data == ".bz2" ||		(Name->data == ".rar" && unrar) )
			goto Next;
		else
			gtk_combo_box_append_text (GTK_COMBO_BOX (combo_box), Name->data );
		Next:
			Name = g_list_next ( Name );
	}
	if (new_combo_box == -1)
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box) , 0 );
	else
		gtk_combo_box_set_active (GTK_COMBO_BOX (combo_box) , new_combo_box );

	gtk_box_pack_start (GTK_BOX (hbox), combo_box, TRUE, TRUE, 0);

	add_extension_cb = gtk_check_button_new_with_label ("Add the archive extension to the filename");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(add_extension_cb),TRUE);
	gtk_box_pack_start (GTK_BOX (hbox), add_extension_cb, TRUE, TRUE, 0);
	gtk_widget_show_all (hbox);
#ifdef GNM_USE_HILDON
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (am_a_file_chooser)->vbox), hbox, FALSE, TRUE, 6);
#else
	gtk_file_chooser_set_extra_widget (GTK_FILE_CHOOSER (am_a_file_chooser), hbox);
#endif

	if (path != NULL)
	{
		basepath = g_path_get_basename (path);
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (am_a_file_chooser),basepath);

		current_dir = g_get_current_dir ();
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (am_a_file_chooser) , current_dir);

		g_free (basepath);
		g_free (current_dir);
	}
	gtk_window_set_modal (GTK_WINDOW (am_a_file_chooser),TRUE);
	if (current_new_directory != NULL)
		gtk_file_chooser_set_current_folder ( GTK_FILE_CHOOSER (am_a_file_chooser) , current_new_directory );
	response = gtk_dialog_run (GTK_DIALOG (am_a_file_chooser));
	current_new_directory = gtk_file_chooser_get_current_folder ( GTK_FILE_CHOOSER (am_a_file_chooser) );

	if ((response == GTK_RESPONSE_ACCEPT) || (response == GTK_RESPONSE_OK))
	{
		my_path = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER (am_a_file_chooser) );
		ComboArchiveType = gtk_combo_box_get_active_text (GTK_COMBO_BOX (combo_box));

		if ( gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (add_extension_cb) ) )
		{
			if ( ! g_str_has_suffix ( my_path , ComboArchiveType ) )
			{
				my_path_ext = g_strconcat ( my_path, ComboArchiveType , NULL);
				g_free (my_path);
				my_path = my_path_ext;
			}
		}
		if ( g_file_test ( my_path , G_FILE_TEST_EXISTS ) )
		{
			gchar *utf8_path;
			gchar  *msg;

			utf8_path = g_filename_to_utf8 (my_path, -1, NULL, NULL, NULL);
			msg = g_strdup_printf ("The archive \"%s\" already exists!", utf8_path);
			response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),
							GTK_DIALOG_MODAL,
							GTK_MESSAGE_QUESTION,
							GTK_BUTTONS_YES_NO,
							msg,
							"Do you want to overwrite it?"
							);
			g_free (utf8_path);
			g_free (msg);
			if (response != GTK_RESPONSE_YES)
			{
				g_free (my_path);
				gtk_widget_destroy (am_a_file_chooser);
			    return NULL;
			}
			unlink ( my_path );
		}
		archive = am_a_init_archive_structure ();
		new_combo_box = gtk_combo_box_get_active (GTK_COMBO_BOX (combo_box));

		if (strcmp ( ComboArchiveType,".rar") == 0)
			archive->type = AM_ARCHIVETYPE_RAR;
		else if (strcmp ( ComboArchiveType,".tar") == 0)
			archive->type = AM_ARCHIVETYPE_TAR;
		else if (strcmp ( ComboArchiveType,".tar.bz2") == 0)
			archive->type = AM_ARCHIVETYPE_TAR_BZ2;
		else if (strcmp ( ComboArchiveType,".tar.gz") == 0)
			archive->type = AM_ARCHIVETYPE_TAR_GZ;
		else if (strcmp ( ComboArchiveType,".zip") == 0 )
			archive->type = AM_ARCHIVETYPE_ZIP;
		else if (strcmp ( ComboArchiveType,".rpm") == 0)
			archive->type = AM_ARCHIVETYPE_RPM;

		gtk_widget_destroy (am_a_file_chooser);
		archive->path = g_strdup (my_path);
		archive->escaped_path = EscapeBadChars (archive->path , "$\'`\"\\!?* ()&|@#:;");
		g_free (my_path);

		return archive;
	}
	else if ( (response == GTK_RESPONSE_CANCEL) || (response == GTK_RESPONSE_DELETE_EVENT) )
		gtk_widget_destroy (am_a_file_chooser);

	return NULL;
}

