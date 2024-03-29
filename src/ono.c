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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "ono-script.h"
#include "ono-utils.h"
#include "ts/scheme-private.h"

#define ONO_VERSON "0.3.0"

/* global interpreter object */
static scheme *ono_interp;

static int  start_repl = 0;
static char *conf_path  = NULL;

static GtkStatusIcon *tray_icon = NULL;

static
void tray_icon_on_click(GtkStatusIcon *status_icon, gpointer user_data) {
	pointer func;
	scheme *s = (scheme*)user_data;

	func = scheme_eval(s, s->vptr->mk_symbol(s, "*ono-on-tray-click*"));
	if(s->vptr->is_closure(func)) {
		scheme_call(s, func, s->NIL);
	}
}

static
void tray_icon_on_menu(GtkStatusIcon *status_icon, guint button,
					   guint activate_time, gpointer menu)
{
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL,
				   gtk_status_icon_position_menu,
				   status_icon,
				   0,
				   gtk_get_current_event_time ());
}

static
void menu_exit(GtkWidget *widget, gpointer user_data) {
	ono_script_fini(ono_interp);
	gtk_main_quit();
}

static
void create_tray_icon(void) {
	GtkWidget *menu, *quit_item, *sep_item;

	/* menu */
	menu = gtk_menu_new();
	quit_item = gtk_image_menu_item_new_with_mnemonic("_Quit");

	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(quit_item),
							  gtk_image_new_from_stock(GTK_STOCK_QUIT, GTK_ICON_SIZE_MENU));

	g_signal_connect(G_OBJECT(quit_item), "activate", G_CALLBACK(menu_exit), NULL);

	ono_menu_append(ONO_MENU(menu), quit_item);
	gtk_widget_show(quit_item);

	sep_item = gtk_separator_menu_item_new();
	ono_menu_prepend(ONO_MENU(menu), sep_item);
	gtk_widget_show(sep_item);

	/* create tray_icon first */
	tray_icon = gtk_status_icon_new();

	/* init interpreter */
	ono_interp = ono_script_init(conf_path, tray_icon);

	/* get user supplied menu */
	menu = ono_script_parse_menu(ono_interp, menu);
	gtk_widget_show(menu);

	/* now setup tray_icon details, as callbacks uses interpreter */
	g_signal_connect(G_OBJECT(tray_icon), "activate",
					 G_CALLBACK(tray_icon_on_click), ono_interp);

	g_signal_connect(G_OBJECT(tray_icon), "popup-menu",
					 G_CALLBACK(tray_icon_on_menu), menu);

	/* default icon, but user can change it */
	gtk_status_icon_set_from_icon_name(tray_icon, "internet-mail");
	gtk_status_icon_set_visible(tray_icon, TRUE);
}

static
void parse_args(int argc, char **argv) {
	struct option options[] = {
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{"repl", no_argument, NULL, 'r'},
		{"config", required_argument, NULL, 'c'},
		{NULL, 0, NULL, 0}
	};

	int ch;
	optarg = NULL;
	optind = 0;
	optopt = 0;

	while((ch = getopt_long(argc, argv, "hvrc:", options, NULL)) != EOF) {
		switch(ch) {
			case 'h': {
				gchar *app = g_path_get_basename(argv[0]);
				g_print("Usage: %s [options]\n"
						"Systray access to offlineimap and more.\n"
						 "Options:\n"
						 "   -h, --help             display this help\n"
						 "   -v, --version          display version\n"
						 "   -r, --repl             start program REPL (shell)\n"
						 "   -c, --config [file]    load file instead $HOME/.onorc\n",
						app);
				g_free(app);
				exit(0);
				break;
			}
			case 'v':
				g_print(ONO_VERSON "\n");
				exit(0);
				break;
			case 'r':
				start_repl = 1;
				break;
			case 'c':
				conf_path = optarg;
				break;
			default:
				g_print("Use '--help' to see options.\n");
				exit(0);
				break;
		}
	}
}

int main(int argc, char **argv) {
	parse_args(argc, argv);

	/* Gtk functions are used in Scheme code, so this must be called in case REPL was invoked. */
	gtk_init(&argc, &argv);

	if(start_repl) {
		ono_interp = ono_script_init(conf_path, NULL);

		g_print("Ono REPL. Type (quit) to exit interpreter.");
		scheme_load_named_file(ono_interp, stdin, "stdin");
		ono_script_fini(ono_interp);
	} else {
		create_tray_icon();
		gtk_main();
	}

	return 0;
}
