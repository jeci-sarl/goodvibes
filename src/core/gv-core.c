/*
 * Goodvibes Radio Player
 *
 * Copyright (C) 2015-2017 Arnaud Rebillout
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <gio/gio.h>

#include "framework/gv-framework.h"

#include "core/gv-engine.h"
#include "core/gv-player.h"
#include "core/gv-station-list.h"

/*
 * Public variables
 */

GApplication  *gv_core_application;
GSettings     *gv_core_settings;

GvStationList *gv_core_station_list;
GvPlayer      *gv_core_player;

gchar         *gv_core_user_agent;

/*
 * Private variables
 */

static GvEngine *gv_core_engine;

static GList *core_objects;

/*
 * Underlying audio backend
 */

#include <gst/gst.h>

#include "additions/gst.h"

void
gv_core_audio_backend_cleanup(void)
{
	if (gst_is_initialized())
		gst_deinit();
}

GOptionGroup *
gv_core_audio_backend_init_get_option_group(void)
{
	return gst_init_get_option_group();
}

const gchar *
gv_core_audio_backend_runtime_version_string(void)
{
	return gst_get_runtime_version_string();
}

const gchar *
gv_core_audio_backend_compile_version_string(void)
{
	return gst_get_compile_version_string();
}

/*
 * Helpers
 */

static gchar *
make_user_agent(void)
{
	return g_strdup_printf("%s/%s (%s)",
	                       PACKAGE_CAMEL_NAME,
	                       PACKAGE_VERSION,
	                       "Linux");
}

/*
 * Core public functions
 */

void
gv_core_quit(void)
{
	g_application_quit(gv_core_application);
}

void
gv_core_configure(void)
{
	GList *item;

	/* The station list must be loaded before any configuration is done.
	 * Otherwise, configure the player current station will fail.
	 */
	gv_station_list_load(gv_core_station_list);

	/* Configure each object that is configurable */
	for (item = core_objects; item; item = item->next) {
		GObject *object = item->data;

		if (!GV_IS_CONFIGURABLE(object))
			continue;

		gv_configurable_configure(GV_CONFIGURABLE(object));
	}
}

void
gv_core_cleanup(void)
{
	/* Destroy core objects */
	core_objects = g_list_reverse(core_objects);
	g_list_foreach(core_objects, (GFunc) g_object_unref, NULL);
	g_list_free(core_objects);

	/* Clear application pointer */
	gv_core_application = NULL;

	/* Free strings */
	g_free(gv_core_user_agent);
}

void
gv_core_init(GApplication *application)
{
	GList *item;

	/* Create strings */
	gv_core_user_agent = make_user_agent();
	DEBUG("User agent: %s", gv_core_user_agent);

	/* Keep a pointer toward application */
	gv_core_application = application;

	/* Create core objects */
	gv_core_settings = g_settings_new(PACKAGE_APPLICATION_ID ".Core");
	core_objects = g_list_append(core_objects, gv_core_settings);

	gv_core_station_list = gv_station_list_new();
	core_objects = g_list_append(core_objects, gv_core_station_list);

	gv_core_engine = gv_engine_new();
	core_objects = g_list_append(core_objects, gv_core_engine);

	gv_core_player = gv_player_new(gv_core_engine, gv_core_station_list);
	core_objects = g_list_append(core_objects, gv_core_player);

	/* Register objects in the framework */
	for (item = core_objects; item; item = item->next) {
		GObject *object = G_OBJECT(item->data);
		gv_framework_register(object);
	}
}
