/*
 * Copyright (C) 2014 Sanel Zukan
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

#include <stdio.h>
#include <stdlib.h>
#include "ts/scheme-private.h"
#include "ono-script.h"

#include "boot_ss.h"

#define SCHEME_RET_IF_FAIL(scm, expr, str)      \
  do {                                          \
	if(!(expr)) {                               \
		g_warning("Error: %s\n", str);			\
		return s->F;							\
	}                                           \
  } while(0)

#define SCHEME_DEFINE(sc, func_ptr, func_name)							\
	sc->vptr->scheme_define(sc, sc->global_env,							\
							sc->vptr->mk_symbol(sc, func_name),			\
							sc->vptr->mk_foreign_func(sc, func_ptr))

static
pointer _system(scheme *s, pointer args) {
	int ret;
	pointer arg = s->vptr->pair_car(args);

	SCHEME_RET_IF_FAIL(s, arg != s->NIL && s->vptr->is_string(arg),
					   "Expected string object as first argument.");

	ret = system(s->vptr->string_value(arg));
	return s->vptr->mk_integer(s, ret);
}

static
pointer _icon_from_theme(scheme *s, pointer args) {
	GtkIconTheme *default_theme;
	GtkIconInfo  *icon_info;
	const char   *icon;
	pointer arg = s->vptr->pair_car(args);

	SCHEME_RET_IF_FAIL(s, arg != s->NIL && s->vptr->is_string(arg),
					   "Expected string object as first argument.");

	icon = s->vptr->string_value(arg);

	/* handle gtk theme stuff */
	default_theme = gtk_icon_theme_get_default();
	icon_info = gtk_icon_theme_lookup_icon(default_theme, icon, 24, 0);

	if(icon_info) {
		const char *path = gtk_icon_info_get_filename(icon_info);
		return s->vptr->mk_string(s, path);
	}

	return s->F;
}

static
void ono_script_init_internals(scheme *s) {
	SCHEME_DEFINE(s, _icon_from_theme, "icon-from-theme");
	SCHEME_DEFINE(s, _system, "system");
}

scheme *ono_script_init(const char *cfile) {
	scheme *scm = NULL;
	char   *conf_home = NULL;
	FILE   *fd;

	scm = scheme_init_new();
	scheme_set_output_port_file(scm, stdout);
	scheme_set_input_port_file(scm, stdin);
	/* interpreter boot stuff */
	scheme_load_string(scm, bundled_scripts_content);
	ono_script_init_internals(scm);

	if(!cfile) {
		conf_home = g_strdup_printf("%s/.onorc", g_get_home_dir());
		cfile = conf_home;
	}

	fd = fopen(cfile, "r");
	if(!fd)
		g_warning("Unable to read configuration from: %s\n", cfile);
	else
		scheme_load_named_file(scm, fd, conf_home ? "~/.onorc" : cfile);

	if(conf_home) g_free(conf_home);
	return scm;
}

void ono_script_fini(scheme *scm) {
	if(scm) {
		scheme_deinit(scm);
		free(scm);
		scm = NULL;
	}
}

GtkWidget *ono_script_parse_menu(scheme *s, GtkWidget *menu) {
	pointer it, user_menu, v, label, icon;
	GtkWidget *menu_item;

	user_menu = scheme_eval(s, s->vptr->mk_symbol(s, "*ono-menu*"));

	for(it = user_menu; it != s->NIL; it = s->vptr->pair_cdr(it)) {
		icon = s->F;
		v = s->vptr->pair_car(it);
		/* each item must be a vector */
		if(!s->vptr->is_vector(v)) continue;

		/* menu item label */
		label = s->vptr->vector_elem(v, 0);
		if(!s->vptr->is_string(label)) continue;

		/* check if there is a icon */
		if(s->vptr->vector_length(v) >= 3) {
			icon = s->vptr->vector_elem(v, 2);
			if(!s->vptr->is_string(icon)) continue;
		}

		menu_item = gtk_image_menu_item_new_with_label(s->vptr->string_value(label));
		if(icon != s->F) {
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
										  gtk_image_new_from_file(s->vptr->string_value(icon)));
		}

		gtk_menu_prepend(GTK_MENU(menu), menu_item);
		gtk_widget_show(menu_item);
	}

	return menu;
}
