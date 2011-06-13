/**
 * @file
 * @author  Tomasz Pieniążek (2010, 2011) <t.pieniazek@gazeta.pl>
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
 * Support for GConf. Used for storing settings, such as selected temporary directory.
 */

#include "gconf.h"

#define AM_NAME "eaman"

#ifdef USE_HILDON
	#define AM_GC_ROOT "/apps/Maemo/"AM_NAME"/"
#else
	#define AM_GC_ROOT "/apps/Gnome/"AM_NAME"/"
#endif

/**
 * Stores selected location for unpacking purposes
 *
 * @param local Our temp location
 *
 */
void confStoreLocation(const gchar* local) {

	GConfClient* gcClient = NULL;
 
	g_assert(local);
 
	gcClient = gconf_client_get_default();
	g_assert(GCONF_IS_CLIENT(gcClient));
	 
	if (!gconf_client_set_string(gcClient, AM_GC_ROOT "tmp_directory", local, NULL)) 
	{
		g_warning(" failed to set %s/tmp_directory to %s\n", AM_GC_ROOT, local);
	}
 
	g_object_unref(gcClient);
	gcClient = NULL;
}

/**
 * Used forsearching the key and getting th value
 *
 * @param gcClient GConf Client
 * @param key Location, where our settings are stored
 * @param vkey The key which we want to search for
 *
 */
void confGetInt(GConfClient* gcClient, const gchar* key, gchar** vkey) {

	GConfValue* val = NULL;
 
	val = gconf_client_get_without_default(gcClient, key, NULL);

	if (val == NULL) {
		g_warning("confGetInt: key %s not found\n", key);
		return FALSE;
	}
 
	if (val->type == GCONF_VALUE_STRING) {
		*vkey = g_strdup(gconf_value_get_string(val));
	} else {
		g_warning("confGetInt: key %s is not an gchar*\n", key);
	}
 
	gconf_value_free(val);
	val = NULL;

}

/**
 * Returns tmp location saved in GConf
 *
 * @return Returns tmp directory
 *
 */
gchar* confLoadCurrentLocation() {
 
	GConfClient* gcClient = NULL;
	gchar *dir = "/tmp";
	 
	gcClient = gconf_client_get_default();
	g_assert(GCONF_IS_CLIENT(gcClient));

	confGetInt(gcClient, AM_GC_ROOT "tmp_directory", &dir);

	g_object_unref(gcClient);
	gcClient = NULL;
 
	return dir;
}

