#
# Copyright (C) 2014 Sanel Zukan
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

PREFIX    ?= /usr/local
BINDIR    ?= $(PREFIX)/bin
MANDIR    ?= $(PREFIX)/man/man1
APPDIR    ?= /usr/share/applications

CC        ?= gcc
GTK_FLAGS ?= $(shell pkg-config gtk+-2.0 --cflags)
GTK_LIBS  ?= $(shell pkg-config gtk+-2.0 --libs)
DEBUG     ?= -g3
CFLAGS    ?= -I. -Isrc -Isrc/ts $(GTK_FLAGS)
LDLIBS    ?= $(GTK_LIBS)

PROGNAME  = src/ono
BOOT_SS   = src/boot_ss.h
SOURCE    = $(wildcard src/*.c) src/ts/scheme.c
OBJECTS   = $(patsubst %.c,%.o, $(SOURCE))

DEPFILE   = .depends
DEPTOKEN  = '\# MAKEDEPENDS'
DEPFLAGS  = -Y -f $(DEPFILE) -s $(DEPTOKEN)

all: $(PROGNAME) $(BOOT_SS)

%.o: %.c
	$(CC) $(CFLAGS) $(DEBUG) -c -o $@ $<

$(BOOT_SS): src/ts/boot.ss
	./tools/gen-c-string.sh $< > $@

$(PROGNAME): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDLIBS)

clean:
	rm -f src/*.o
	rm -f $(PROGNAME)
	rm -f src/ts/*.o

man:
	txt2man -t ono -s 1 doc/ono.man > doc/ono.1

install: $(PROGNAME)
	cp $(PROGNAME) $(BINDIR)
	cp doc/ono.1 $(MANDIR)
	cp ono.desktop $(APPDIR)

uninstall:
	rm -f $(BINDIR)/$(PROGNAME)
	rm -f $(MANDIR)/ono.1
	rm -f $(APPDIR)/ono.desktop

# dependency management
$(DEPFILE):
	@touch $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(SOURCE) >&/dev/null

depend:
	rm -f $(DEPFILE)
	make $(DEPFILE)

sinclude $(DEPFILE)
