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

static GtkStatusIcon *tray_icon = NULL;

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

static void menu_item_callback(GtkMenuItem *widget, gpointer user_data);

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
pointer _system(scheme *s, pointer args) {
	int ret;
	pointer arg = s->vptr->pair_car(args);

	SCHEME_RET_IF_FAIL(s, arg != s->NIL && s->vptr->is_string(arg),
					   "Expected string object as first argument.");

	ret = system(s->vptr->string_value(arg));
	return s->vptr->mk_integer(s, ret);
}

static
pointer _system_with_reader(scheme *s, pointer args) {
	pointer arg;
	const char *cmd;
	FILE *fd;

	arg = s->vptr->pair_car(args);
	SCHEME_RET_IF_FAIL(s, arg != s->NIL && s->vptr->is_string(arg),
					   "Expected string object as first argument.");

	cmd = s->vptr->string_value(arg);

	args = s->vptr->pair_cdr(args);
	arg  = s->vptr->pair_car(args);
	SCHEME_RET_IF_FAIL(s, arg != s->NIL && s->vptr->is_closure(arg),
					   "Expected function as the second argument.");

	fd = popen(cmd, "r");
	if(fd) {
		char buf[1024];
		pointer val;

		while((fgets(buf, sizeof(buf), fd)) != NULL) {
			val = s->vptr->mk_string(s, buf);
			scheme_call(s, arg, cons(s, val, s->NIL));
		}

		pclose(fd);
		return s->T;
	}

	g_warning("Unable to execute command: %s\n", cmd);
	return s->F;
}

static
pointer _ono_icon_set(scheme *s, pointer args) {
	int ret;
	pointer arg = s->vptr->pair_car(args);

	SCHEME_RET_IF_FAIL(s, arg != s->NIL && s->vptr->is_string(arg),
					   "Expected string object as first argument.");

	if(tray_icon) {
		gtk_status_icon_set_visible(tray_icon, FALSE);
		gtk_status_icon_set_from_icon_name(tray_icon, s->vptr->string_value(arg));
		gtk_status_icon_set_visible(tray_icon, TRUE);
		return s->T;
	}

	return s->F;
}

static
pointer _ono_tooltip_set(scheme *s, pointer args) {
	int ret;
	pointer arg = s->vptr->pair_car(args);

	SCHEME_RET_IF_FAIL(s, arg != s->NIL && s->vptr->is_string(arg),
					   "Expected string object as first argument.");

	if(tray_icon) {
		gtk_status_icon_set_tooltip(tray_icon, s->vptr->string_value(arg));
		return s->T;
	}

	return s->F;
}

static
void ono_script_init_internals(scheme *s) {
	SCHEME_DEFINE(s, _icon_from_theme, "icon-from-theme");
	SCHEME_DEFINE(s, _system, "system");
	SCHEME_DEFINE(s, _system_with_reader, "system-with-reader");

	/* valid only in GUI (not REPL) */
	SCHEME_DEFINE(s, _ono_icon_set, "ono-icon");
	SCHEME_DEFINE(s, _ono_tooltip_set, "ono-tooltip");
}

scheme *ono_script_init(const char *cfile, GtkStatusIcon *icon) {
	scheme *scm = NULL;
	char   *conf_home = NULL;
	FILE   *fd;

	tray_icon = icon;
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

/*
 * to deduplicate, this function will either build menu from *ono-menu* variable
 * or run menu item callback, associated with given label
 */
static GtkWidget *fill_or_run(scheme *s, GtkWidget *menu, const char *find_label) {
	pointer it, user_menu, v, label, icon;
	GtkWidget *menu_item;
	const char *mlabel;

	user_menu = scheme_eval(s, s->vptr->mk_symbol(s, "*ono-menu*"));

	for(it = user_menu; it != s->NIL; it = s->vptr->pair_cdr(it)) {
		icon = s->F;
		v = s->vptr->pair_car(it);
		/* each item must be a vector */
		if(!s->vptr->is_vector(v)) continue;

		/* and has to have at least 2 elements, label and callback */
		if(s->vptr->vector_length(v) < 2) continue;

		/* menu item label */
		label = s->vptr->vector_elem(v, 0);
		if(!s->vptr->is_string(label)) continue;

		/* check if there is a icon */
		if(s->vptr->vector_length(v) >= 3) {
			icon = s->vptr->vector_elem(v, 2);
			if(!s->vptr->is_string(icon)) continue;
		}

		mlabel = s->vptr->string_value(label);

		if(menu) {
			menu_item = gtk_image_menu_item_new_with_label(mlabel);
			if(icon != s->F) {
				gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
											  gtk_image_new_from_file(s->vptr->string_value(icon)));
			}

			g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(menu_item_callback), s);
			gtk_menu_prepend(GTK_MENU(menu), menu_item);
			gtk_widget_show(menu_item);

		} else if(g_strcmp0(mlabel, find_label) == 0) {
			pointer func = s->vptr->vector_elem(v, 1);
			if(s->vptr->is_closure(func))
				scheme_call(s, func, s->NIL);
		}
	}

	return menu;
}

static
void menu_item_scheme_callback(scheme *s, const char *label) {
	fill_or_run(s, NULL, label);
}

static
void menu_item_callback(GtkMenuItem *widget, gpointer user_data) {
	const gchar *label = gtk_menu_item_get_label(GTK_MENU_ITEM(widget));
	scheme *s = (scheme*)user_data;
	menu_item_scheme_callback(s, (const char*)label);
}

GtkWidget *ono_script_parse_menu(scheme *s, GtkWidget *menu) {
	return fill_or_run(s, menu, NULL);
}
