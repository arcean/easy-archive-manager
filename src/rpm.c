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
 * Support for RPM packages.
 */

#include "config.h"
#include "rpm.h"
#include "utils.h"

extern int output_fd, input_fd;
FILE *stream;
int fd;
gchar *cpio_tmp = NULL;
gchar *tmp = NULL;
gchar buffer[2048];
gsize bytes_read = 0;
gsize bytes_written = 0;
GIOStatus status;
GError *error = NULL;
GIOChannel *ioc_cpio , *input_ioc, *output_ioc;

/**
 * Function used to opening RPM packages
 *
 * @param archive Archive we want to open
 *
 */
void OpenRPM ( AM_Archive *archive )
{
	unsigned char bytes[8];
	int dl,il,sigsize,offset;
	gchar *ibs;

	signal (SIGPIPE, SIG_IGN);
	stream = fopen ( archive->path , "r" );

	if (stream == NULL)
	{
        	gchar *msg = g_strdup_printf ("Can't open RPM file %s:" , archive->path);
		response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow) , GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,
			msg,g_strerror (errno));
		g_free (msg);
		return;
	}

	archive->dummy_size = 0;
	archive->nr_of_files = 0;
	archive->nr_of_dirs = 0;
	archive->format ="RPM";
	char *names[]= {("Filename"),("Permission"),("Symbolic Link"),("Hard Link"),("Owner"),("Group"),("Size")};
	GType types[]= {G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_UINT64};
	am_a_create_liststore ( 7, names , (GType *)types, archive );
	if (fseek ( stream, 104 , SEEK_CUR ) )
	{
        	fclose (stream);
        	response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't fseek to position 104:",g_strerror(errno));
        	return;
	}
    	if ( fread ( bytes, 1, 8, stream ) == 0 )
	{
		fclose ( stream );
		response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't read data from file:",g_strerror(errno));
		return;
	}
	il = 256 * ( 256 * ( 256 * bytes[0] + bytes[1]) + bytes[2] ) + bytes[3];
	dl = 256 * ( 256 * ( 256 * bytes[4] + bytes[5]) + bytes[6] ) + bytes[7];
	sigsize = 8 + 16 * il + dl;
	offset = 104 + sigsize + ( 8 - ( sigsize % 8 ) ) % 8 + 8;
	if (fseek ( stream, offset  , SEEK_SET ) )
	{
        	fclose (stream);
        	response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't fseek in file:",g_strerror(errno));
        	return;
	}
	if ( fread ( bytes, 1, 8, stream ) == 0 )
	{
		fclose ( stream );
		response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't read data from file:",g_strerror(errno));
		return;
	}

	il = 256 * ( 256 * ( 256 * bytes[0] + bytes[1]) + bytes[2] ) + bytes[3];
 	dl = 256 * ( 256 * ( 256 * bytes[4] + bytes[5]) + bytes[6] ) + bytes[7];
	sigsize = 8 + 16 * il + dl;
	offset = offset + sigsize;
	fclose (stream);

//	cpio_tmp = g_strdup ("/tmp/archiver-XXXXXX");
	cpio_tmp = g_strconcat(whereToExtract, "/archiver-XXXXXX", NULL);
	fd = g_mkstemp ( cpio_tmp );
	ibs = g_strdup_printf ( "%u" , offset );

	gchar *command = g_strconcat ( "dd if=" , archive->escaped_path, " ibs=" , ibs , " skip=1 of=" , cpio_tmp , NULL );
	g_free (ibs);
	archive->parse_output = 0;
	SpawnAsyncProcess ( archive , command , 0, 0);
	g_free ( command );
	if ( archive->child_pid == 0 )
	{
		fclose ( stream );
		unlink ( cpio_tmp );
		g_free (cpio_tmp);
		return;
	}
	g_child_watch_add ( archive->child_pid , (GChildWatchFunc) DecompressCPIO , cpio_tmp );
}

/**
 * Function used to decompress package's CPIO
 *
 */
GChildWatchFunc *DecompressCPIO (GPid pid , gint status , gpointer data)
{
	gint current_page;

	current_page = gtk_notebook_get_current_page(notebook);
	gchar *gzip = data;
	if ( WIFEXITED(status) )
	{
		if ( WEXITSTATUS (status) )
		{
	    		am_a_set_window_title (MainWindow , NULL);
		    	response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Error while extracting the cpio archive from the rpm one.","Do you want to view the command line output?");
			if (response == GTK_RESPONSE_YES)
				am_a_show_cmd_line_output (NULL);
            		unlink ( cpio_tmp );
            		g_free (cpio_tmp);
            		return FALSE;
    		}
    	}
	cpio_tmp = am_a_open_temp_file ( gzip );
    	if (cpio_tmp != NULL)
		g_child_watch_add ( archive[current_page]->child_pid , (GChildWatchFunc) OpenCPIO , gzip );
    	else
		return FALSE;
    	return NULL;
}

GChildWatchFunc *OpenCPIO (GPid pid , gint exit_code , gpointer data)
{
	gint current_page;

	current_page = gtk_notebook_get_current_page(notebook);
	gchar *gzip = data;
    	if ( WIFEXITED( exit_code ) )
    	{
	    	if ( WEXITSTATUS ( exit_code ) )
    	{
            gtk_widget_hide ( viewport2 );
	    	am_a_set_window_title (MainWindow , NULL);
		    response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"An error occurred while decompressing the cpio archive.","Do you want to view the command line output?" );
			if (response == GTK_RESPONSE_YES)
				am_a_show_cmd_line_output (NULL);
			unlink ( cpio_tmp );
			unlink ( gzip );
			g_free (cpio_tmp);
			return FALSE;
		}
	}
	/* Let's delete the tmp gzip compressed CPIO archive since we don't need it anymore */
	unlink ( gzip );

	/* Now I have to open the CPIO temp file in read mode and spawn the
	command cpio -tv with an input pipe so to receive the output from
	the opened CPIO temp file */

	archive[current_page]->parse_output = 0;
	SpawnAsyncProcess ( archive[current_page] , "cpio -tv" , 1, 0 );
	if ( archive[current_page]->child_pid == 0 )
	{
		unlink ( cpio_tmp );
		g_free ( cpio_tmp );
		return FALSE;
	}
	output_ioc = g_io_channel_unix_new ( output_fd );
	g_io_add_watch (output_ioc, G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL, ReadCPIOOutput, archive[current_page] );
	g_io_channel_set_encoding (output_ioc, locale , NULL);
	g_io_channel_set_flags ( output_ioc , G_IO_FLAG_NONBLOCK , NULL );

	input_ioc = g_io_channel_unix_new ( input_fd );
	g_io_add_watch (input_ioc, G_IO_IN|G_IO_OUT|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL, WriteCPIOInput, NULL );
	g_io_channel_set_encoding (input_ioc, NULL , NULL);

	ioc_cpio = g_io_channel_new_file ( cpio_tmp , "r" , NULL );
	g_io_channel_set_encoding (ioc_cpio , NULL , NULL);
	g_io_channel_set_flags ( ioc_cpio , G_IO_FLAG_NONBLOCK , NULL );

	g_child_watch_add ( archive[current_page]->child_pid, (GChildWatchFunc) am_a_watch_child, archive[current_page]);
  return NULL;
}

/* input pipe */
gboolean WriteCPIOInput (GIOChannel *ioc, GIOCondition cond, gpointer data)
{
	if (cond & (G_IO_IN | G_IO_PRI | G_IO_OUT) )
    {
		/* Doing so I write to the input pipe of the g_spawned "cpio -tv" so to produce the list of files in the cpio archive */
		status = g_io_channel_read_chars ( ioc_cpio , buffer, sizeof(buffer), &bytes_read, &error);
		if ( status != G_IO_STATUS_EOF)
		{
			status = g_io_channel_write_chars ( ioc , buffer , bytes_read , &bytes_written , &error );
			if (status == G_IO_STATUS_ERROR)
			{
				response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"An error occurred:",error->message);
				g_error_free (error);
				CloseChannels ( ioc_cpio );
				CloseChannels ( ioc );
				return FALSE;
			}
			/*
			while ( bytes_read != bytes_written )
				status = g_io_channel_write_chars ( ioc , buffer + bytes_written , bytes_read - bytes_written , &bytes_written , &error );
			g_print ("*Written:%d\tStatus:%d\n",bytes_written,status);
			*/
			g_io_channel_flush ( ioc , NULL );
			return TRUE;
		}
		else
		{
			CloseChannels ( ioc_cpio );
			CloseChannels ( ioc );
			return FALSE;
		}
	}
  return TRUE;
}

/**
 * Internal function, used by the others to read informations about RPM structure
 *
 */
gboolean ReadCPIOOutput (GIOChannel *ioc, GIOCondition cond, gpointer data)
{
	indicateProcessingData(TRUE);
	AM_Archive *archive = data;
	gchar **fields = NULL;
    	gchar *line = NULL;
    	gchar *filename = NULL;
	GIOStatus status = G_IO_STATUS_NORMAL;
	GtkTreeIter iter;
	gchar *temp = NULL;

	if (cond & (G_IO_IN | G_IO_PRI) )
	{
		do
		{
			status = g_io_channel_read_line ( ioc, &line, NULL, NULL , NULL );
			if (line == NULL)
				break;

			fields = split_line (line , 5);
			filename = get_last_field (line , 9);
			gtk_list_store_append (archive->liststore, &iter);
			if ( g_str_has_prefix(fields[0] , "d") == FALSE)
				archive->nr_of_files++;
            else
				archive->nr_of_dirs++;

			temp = g_strrstr (filename,"->");
			if (temp)
			{
				gtk_list_store_set (archive->liststore, &iter,2,g_strstrip(&temp[3]),-1);
				temp = g_strstrip(g_strndup(filename, strlen(filename) - strlen(temp) ));
				gtk_list_store_set (archive->liststore, &iter,0,temp,-1);
				g_free (temp);
			}
			else
			{
				gtk_list_store_set (archive->liststore, &iter,2,NULL,-1);
				gtk_list_store_set (archive->liststore, &iter,0,filename,-1);
			}

			gtk_list_store_set (archive->liststore, &iter,1,fields[0],-1);
			gtk_list_store_set (archive->liststore, &iter,3,fields[1],-1);
			gtk_list_store_set (archive->liststore, &iter,4,fields[2],-1);
			gtk_list_store_set (archive->liststore, &iter,5,fields[3],-1);
			gtk_list_store_set (archive->liststore, &iter,6,strtoll(fields[4],NULL,0),-1);

            while (gtk_events_pending() )
				gtk_main_iteration();

            archive->dummy_size += strtoll(fields[4],NULL,0);
            g_strfreev ( fields );
			g_free (line);
		}
		while (status == G_IO_STATUS_NORMAL);

		if (status == G_IO_STATUS_ERROR || status == G_IO_STATUS_EOF)
			goto done;
	}
	else if (cond & (G_IO_ERR | G_IO_HUP | G_IO_NVAL) )
	{
done:	CloseChannels (ioc);
		gtk_tree_view_set_model (GTK_TREE_VIEW(archive->treeview), archive->model);
		g_object_unref (archive->model);
		archive->tmp = cpio_tmp;
		indicateProcessingData(FALSE);
		return FALSE;
	}
	indicateProcessingData(FALSE);
	return TRUE;
}

void CloseChannels ( GIOChannel *ioc )
{
    g_io_channel_shutdown ( ioc,TRUE,NULL );
    g_io_channel_unref (ioc);
}

/**
 * Internal function, used by the others to extract RPM package
 *
 */
gchar *am_a_open_temp_file ( gchar *temp_path )
{
	gint current_page;

	current_page = gtk_notebook_get_current_page(notebook);
	gchar *command = NULL;
	//tmp = g_strdup ("/tmp/archiver-XXXXXX");
	tmp = g_strconcat(whereToExtract, "/archiver-XXXXXX", NULL);

	fd = g_mkstemp ( tmp );
	stream = fdopen ( fd , "w" );
	if ( stream == NULL)
	{
		response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,"Can't write to /tmp:",g_strerror(errno) );
		g_free (tmp);
		return NULL;
	}
	if (am_a_detect_archive_type ( NULL , temp_path ) == AM_ARCHIVETYPE_GZIP)
		command = g_strconcat ( "gzip -dc " , temp_path , NULL );
	else
		command = g_strconcat ( "bzip2 -dc " , temp_path , NULL );
	archive[current_page]->parse_output = 0;
	SpawnAsyncProcess ( archive[current_page] , command , 0, 0);
	g_free ( command );
	if ( archive[current_page]->child_pid == 0 )
	{
		fclose ( stream );
		unlink ( tmp );
		g_free (tmp);
		return NULL;
	}
	GIOChannel *ioc = g_io_channel_unix_new ( output_fd );
	g_io_channel_set_encoding (ioc, NULL , NULL);
	g_io_channel_set_flags ( ioc , G_IO_FLAG_NONBLOCK , NULL );
	g_io_add_watch (ioc, G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP|G_IO_NVAL, ExtractToDifferentLocation, stream);
	return tmp;
}

/**
 * Internal function, used by the others to extract RPM package to selected location
 *
 */
gboolean ExtractToDifferentLocation (GIOChannel *ioc, GIOCondition cond, gpointer data)
{
	FILE *stream = data;
	gchar buffer[65536];
	gsize bytes_read;
	GIOStatus status;
	GError *error = NULL;

	if (cond & (G_IO_IN | G_IO_PRI) )
	{
		do
	    {
			status = g_io_channel_read_chars (ioc, buffer, sizeof(buffer), &bytes_read, &error);
			if (bytes_read > 0)
			{
				fwrite (buffer, 1, bytes_read, stream);
			}
			else if (error != NULL)
			{
			response = ShowGtkMessageDialog (GTK_WINDOW (MainWindow),GTK_DIALOG_MODAL,GTK_MESSAGE_ERROR,GTK_BUTTONS_OK, "An error occurred:",error->message);
			g_error_free (error);
			return FALSE;
			}
		}
		while (status == G_IO_STATUS_NORMAL);

		if (status == G_IO_STATUS_ERROR || status == G_IO_STATUS_EOF)
		goto done;
	}
	else if (cond & (G_IO_ERR | G_IO_HUP | G_IO_NVAL) )
	{
		done:
		fclose ( stream );
		g_io_channel_shutdown ( ioc,TRUE,NULL );
		g_io_channel_unref (ioc);
		return FALSE;
	}
	return TRUE;
}

