# ono 

Small application that sits inside system tray, allowing you to
manage [Offlineimap](http://offlineimap.org) and mail agent easily.

<p align="center">
<img src="https://raw.github.com/sanel/ono/master/images/shot.png"
     alt="ono" title="ono screenshot">
</p>

## Introduction

*ono* is free, configurable and portable GUI application from
where you can easily handle Offlineimap and start any mail agent
(including terminal agents, like [Mutt](http://www.mutt.org)).

Thanks to builtin scripting language, you can even abuse *ono* to
serve any program without system tray support.

## Installation & running

Make sure you have C compiler, GNU make and Gtk+2 installed
(compilation under Gtk+3 isn't tested, but should work out of the
box).

Download latest official release or clone this repository (if you
would like to have bleeding edge code), and inside *ono* folder run:

```
$ make
```

To install it, as superuser (or root) run:

```
$ make install
```

If you happen to have permissions to install as normal user using
*sudo*, this will work too:

```
$ sudo make install
```

Before running it, make sure to configure it first. The next section
will explain you the details behind configuration and how to adjust it
to suit your needs.

## Configuration

*ono* comes with sample configuration file named **onorc**
(distributed with the source code, but not installed by
default). You have to copy it to *$HOME/.onorc* and configure to match
your Offlineimap configuration.

Configuration language *ono* is using is
[Scheme](http://en.wikipedia.org/wiki/Scheme_(programming_language)),
better say a subset of R5RS specification, which gives *ono* enough
flexibility to be extended and modified without writing C code.

If you never used Scheme before, no worry; this is very simple
language easy to learn and there are numerous tutorials online for
beginners. For example
[Yet Another Scheme Tutorial](http://www.shido.info/lisp/idx_scm_e.html)
is really nice introduction to Scheme.

**onorc** is heavily commented, but here is example how it can look
like:

```scheme
; this is a comment

;(set! *offlineimap-path* "/opt/offlineimap/bin/offlineimap")
;(set! *offlineimap-args* "-u quiet")

(ono-menu
  (vector "Fetch yahoo" "yahoo")
  (vector "Fetch gmail" "gmail")
  (vector "Fetch All" (lambda () (offlineimap #f #f))))

(ono-on-tray-click
 (lambda ()
   (system "mrxvt -m -e mutt")))
```

_*offlineimap-path*_ should contain Offlineimap program location. By
default, you don't have to set this variable (*ono* will try to search
it on your _$PATH_) unless you have installed Offlineimap on unusual
location.

_*offlineimap-args*_ are global Offlineimap command line arguments
used for all accounts. With *ono* you can set account specific or
global arguments (arguments Offlineimap understainds, of course).

*ono-menu* is a construct where you put menu items, displayed when
right mouse button is clicked on *ono* system tray icon. Each entry
has label name, Offlineimap account name (or Scheme function) and
optional icon, assigned to given menu item.

For example, statement:

```scheme
  (vector "Fetch yahoo" "yahoo")
```

tells *ono* to add menu entry labeled as "Fetch yahoo" and assign
action; in this case is to invoke Offlineimap with *yahoo* account
(just like you would say in command line: *offlineime -a yahoo*).

Action can be either Scheme string or function. In case of string,
*ono* will assume you have Offlineimap account with the same name or
will not be able to run Offlineimap successfully. However, if you
would like more advanced control, you should use a function.

## License

Copyright (c) Sanel Zukan. GPL v2.
