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
 * Additional set of functions operating on strings.
 */

#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>
#include "utils.h"

#define APPLICATION_NAME "Easy Archive Manager"

static int count_chars_to_escape (const char *str, const char *meta_chars)
{
        int         meta_chars_n = strlen (meta_chars);
        const char *s;
        int         n = 0;

        for (s = str; *s != 0; s++) {
                int i;
                for (i = 0; i < meta_chars_n; i++)
                        if (*s == meta_chars[i]) {
                                n++;
                                break;
                        }
        }
        return n;
}

char *escape_str_common (const char *str, const char *meta_chars, const char  prefix, const char  postfix)
{
        int         meta_chars_n = strlen (meta_chars);
        char       *escaped;
        int         i, new_l, extra_chars = 0;
        const char *s;
        char       *t;

        if (str == NULL)
                return NULL;

        if (prefix)
                extra_chars++;
        if (postfix)
                extra_chars++;

        new_l = strlen (str) + (count_chars_to_escape (str, meta_chars) * extra_chars);
        escaped = g_malloc (new_l + 1);

        s = str;
        t = escaped;
        while (*s) {
                gboolean is_bad = FALSE;
                for (i = 0; (i < meta_chars_n) && !is_bad; i++)
                        is_bad = (*s == meta_chars[i]);
                if (is_bad && prefix)
                        *t++ = prefix;
                *t++ = *s++;
                if (is_bad && postfix)
                        *t++ = postfix;
        }
        *t = 0;

        return escaped;
}

gchar *EscapeBadChars ( gchar *string , gchar *pattern)
{
	return escape_str_common (string, pattern, '\\', 0);
}

int CountCharacter ( gchar *string , int chr )
{
    int n = 0;
    while ( *string )
    {
        if ( *string == chr ) n++;
        string++;
    }
    return n;
}

gchar *RemoveBackSlashes ( gchar *name)
{
    gchar *nome, *q;
    int x = CountCharacter ( name , '\\' );
    nome = (char *) g_malloc (strlen(name) - x + 1);
    q = nome;
    while ( *name )
    {
        if ( *name == '\\' ) name++;
        *q++ = *name++;
    }
    *q = '\000';
    return nome;
}

char *get_last_field (char *line,int last_field)
{
	char *field;
	int i;

	if (line == NULL)
		return NULL;

	last_field--;
	field = eat_spaces (line);
	for (i = 0; i < last_field; i++) {
		if (field == NULL)
			return NULL;
		field = strchr (field, ' ');
		field = eat_spaces (field);
	}
    if (field != NULL) field [ strlen(field) -1 ] = '\000';
	return field;
}

char **split_line (char *line,int n_fields)
{
	char **fields;
	char *scan, *field_end;
	int i;

	fields = g_new0 (char *, n_fields + 1);
	fields[n_fields] = NULL;

	scan = eat_spaces (line);
	for (i = 0; i < n_fields; i++)
	{
		if (scan == NULL)
		{
			fields[i] = NULL;
			continue;
		}
		field_end = strchr (scan, ' ');
		if (field_end == NULL) field_end = strchr (scan, '\n');
		if (field_end != NULL)
		{
			fields[i] = g_strndup (scan, field_end - scan);
			scan = eat_spaces (field_end);
		}
	}
	return fields;
}

char *eat_spaces (char *line)
{
	if (line == NULL)
		return NULL;
	while ((*line == ' ') && (*line != 0))
		line++;
	return line;
}

gchar *remove_level_from_path (const gchar *path)
{
    const gchar *ptr = path;
    gint p;
    if (! path) return NULL;
    p = strlen (path) - 1;
    if (p < 0) return NULL;
    while ((ptr[p] != '/') && (p > 0))
        p--;
    if ((p == 0) && (ptr[p] == '/')) p++;
    return g_strndup (path, (guint)p);
}

gboolean file_extension_is (const char *filename, const char *ext)
{
	int filename_l, ext_l;

	filename_l = strlen (filename);
	ext_l = strlen (ext);

    	if (filename_l < ext_l)
		return FALSE;
    	return strcasecmp (filename + filename_l - ext_l, ext) == 0;
}

void am_a_set_window_title ( GtkWidget *window , gchar *title)
{
	gchar *x 	= NULL;
	gchar *slash= NULL;

	if (title == NULL)
		gtk_window_set_title ( GTK_WINDOW (window) , APPLICATION_NAME );
	else
	{
		slash = g_strrstr (title , "/");
		if (slash == NULL)
		{
			x = g_strconcat (  title , " - " , APPLICATION_NAME , NULL);
			gtk_window_set_title ( GTK_WINDOW (window) , x);
			g_free (x);
			return;
		}
		else
		{
			x = g_strconcat (  slash , " - " , APPLICATION_NAME, NULL);
			x++;
			gtk_window_set_title ( GTK_WINDOW (window) , x);
			x--;
			g_free (x);
		}
	}
}
