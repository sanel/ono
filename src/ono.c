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
#include <getopt.h>
#include "ono-script.h"

/* global interpreter object */
static scheme *ono_interp;
static int     start_repl = 0;
static char   *conf_path  = NULL;

static
void tray_icon_on_click(GtkStatusIcon *status_icon, 
                        gpointer user_data)
{
	printf("Clicked on tray icon\n");
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
void menu_exit(gchar *str) {
	ono_script_fini(ono_interp);
	gtk_main_quit();
}

static
void create_tray_icon(void) {
	GtkStatusIcon *tray_icon;
	GtkWidget *menu, *quit_item, *sep_item;

	/* menu */
	menu = gtk_menu_new();
	quit_item = gtk_image_menu_item_new_with_mnemonic("_Quit");

	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(quit_item),
							  gtk_image_new_from_stock(GTK_STOCK_QUIT, GTK_ICON_SIZE_MENU));
	gtk_signal_connect_object(GTK_OBJECT(quit_item), "activate",
							  GTK_SIGNAL_FUNC(menu_exit), NULL);
	gtk_menu_append(GTK_MENU(menu), quit_item);
	gtk_widget_show(quit_item);

	sep_item = gtk_separator_menu_item_new();
	gtk_menu_prepend(GTK_MENU(menu), sep_item);
	gtk_widget_show(sep_item);

	/* init interpreter and build user menus if needed */
	ono_interp = ono_script_init(conf_path, menu);

	gtk_widget_show(menu);

	/* tray icon */
	tray_icon = gtk_status_icon_new();
	g_signal_connect(G_OBJECT(tray_icon), "activate", 
					 G_CALLBACK(tray_icon_on_click), NULL);

	g_signal_connect(G_OBJECT(tray_icon), "popup-menu",
					 G_CALLBACK(tray_icon_on_menu), menu);

	gtk_status_icon_set_from_icon_name(tray_icon, "internet-mail");
	gtk_status_icon_set_tooltip(tray_icon, "Offlineimap status");
	gtk_status_icon_set_visible(tray_icon, TRUE);
}

static
void parse_args(int argc, char **argv) {
	struct option options[] = {
		{"help", no_argument, NULL, 'h'},
		{"repl", no_argument, NULL, 'r'},
		{"config", required_argument, NULL, 'c'},
		{NULL, 0, NULL, 0}
	};

	int ch;
	optarg = NULL;
	optind = 0;
	optopt = 0;

	while((ch = getopt_long(argc, argv, "hrc:", options, NULL)) != EOF) {
		switch(ch) {
			case 'h': {
				gchar *app = g_path_get_basename(argv[0]);
				g_printf("Usage: %s [options]\n"
						 "Systray access to offlineima and more.\n"
						 "Options:\n"
						 "    --help             display this help\n"
						 "    --repl             start program REPL (shell)\n"
						 "    --config [file]    load file instead $HOME/.onorc\n",
						app);
				g_free(app);
				exit(0);
				break;
			}
			case 'r':
				start_repl = 1;
				break;
			case 'c':
				conf_path = optarg;
				break;
			default:
				g_printf("Use '--help' to see options.\n");
				exit(0);
				break;
		}
	}
}

int main(int argc, char **argv) {
	GtkStatusIcon *tray_icon;
	parse_args(argc, argv);

	if(start_repl) {
		ono_interp = ono_script_init(conf_path, NULL);

		g_printf("Ono REPL. Type (quit) to exit interpreter.");
		scheme_load_named_file(ono_interp, stdin, "stdin");
		ono_script_fini(ono_interp);
	} else {
		gtk_init(&argc, &argv);
		create_tray_icon();
		gtk_main();
	}
	return 0;
}
