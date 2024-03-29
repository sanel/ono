/*
 * Copyright (C) 2014-2022 Sanel Zukan
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __ONO_SCRIPT_H__
#define __ONO_SCRIPT_H__

#include <gtk/gtk.h>
#include "ts/scheme.h"

scheme *ono_script_init(const char *cfile, GtkStatusIcon *icon);
void    ono_script_fini(scheme *scm);

GtkWidget *ono_script_parse_menu(scheme *scm, GtkWidget *menu);
#endif
