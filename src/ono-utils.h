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

#ifndef __ONO_UTILS_H__
#define __ONO_UTILS_H__

#include <gtk/gtk.h>

#ifdef USE_GTK3
# define ONO_MENU GTK_MENU_SHELL
# define ono_menu_append  gtk_menu_shell_append
# define ono_menu_prepend gtk_menu_shell_prepend
# define ono_status_icon_set_tooltip gtk_status_icon_set_tooltip_text
#else
# define ONO_MENU GTK_MENU
# define ono_menu_append  gtk_menu_append
# define ono_menu_prepend gtk_menu_prepend
# define ono_status_icon_set_tooltip gtk_status_icon_set_tooltip
#endif

#endif
