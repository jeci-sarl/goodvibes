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

#include <glib-object.h>

#include "framework/log.h"
#include "framework/gv-configurable.h"

/*
 * GObject definitions
 */

G_DEFINE_INTERFACE(GvConfigurable, gv_configurable, G_TYPE_OBJECT)

/*
 * Public methods
 */

void
gv_configurable_configure(GvConfigurable *self)
{
	g_return_if_fail(GV_IS_CONFIGURABLE(self));
	return GV_CONFIGURABLE_GET_IFACE(self)->configure(self);
}

/*
 * GObject methods
 */

static void
gv_configurable_default_init(GvConfigurableInterface *iface)
{
	TRACE("%p", iface);
}
