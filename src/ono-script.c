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
#include "ono-script.h"

#include "boot_ss.h"

scheme *ono_script_init(const char *cfile, GtkWidget *menu) {
	scheme *scm = NULL;
	char   *conf_home = NULL;
	FILE   *fd;

	scm = scheme_init_new();
	scheme_set_output_port_file(scm, stdout);
	scheme_set_input_port_file(scm, stdin);
	/* interpreter boot stuff */
	scheme_load_string(scm, bundled_scripts_content);

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
