/*
 *  Copyright (c) 2010,2011 Tomasz Pieniążek <t.pieniazek@gazeta.pl>
 *  Copyright (c) 2005,2006 Giuseppe Torelli - <colossus73@gmail.com>
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
 */

#ifndef UTILS_H
#define UTILS_H

gchar *EscapeBadChars ( gchar *string , gchar *pattern);
char *escape_str_common (const char *str, const char *meta_chars, const char  prefix, const char  postfix);
char *eat_spaces (char *line);
gchar *remove_level_from_path (const gchar *path);
int CountCharacter ( gchar *string , int chr );
gchar *RemoveBackSlashes ( gchar *name);
char *get_last_field (char *line,int last_field);
char **split_line (char *line,int n_fields);
gboolean file_extension_is (const char *filename, const char *ext);
void am_a_set_window_title ( GtkWidget *window , gchar *title);

#endif
