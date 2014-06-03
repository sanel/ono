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

[Download](https://github.com/sanel/ono/releases) latest official
release or clone this repository (if you would like to have bleeding
edge code), and inside *ono* folder run:

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
flexibility to be extended and modified without writing additional C
code.

If you never used Scheme before, no worry; Scheme is very simple
language easy to learn and there are numerous tutorials online for
beginners; for example
[Yet Another Scheme Tutorial](http://www.shido.info/lisp/idx_scm_e.html).

**onorc** is heavily commented, but here is how it can look like:

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

***offlineimap-path*** should contain Offlineimap program location. By
default, you don't have to set this variable (*ono* will try to search
it on your _$PATH_) unless you have installed Offlineimap on unusual
location.

***offlineimap-args*** are global Offlineimap command line arguments
used for all accounts. With *ono* you can set account specific or
global arguments (arguments Offlineimap understands, of course).

**ono-menu** is a construct where you put menu items, displayed when
right mouse button is clicked on *ono* system tray icon. Each entry
has label name, Offlineimap account name (or Scheme function) and
optional icon, assigned to given menu item.

For example, the statement:

```scheme
  (vector "Fetch yahoo" "yahoo")
```

tells *ono* to add menu entry labeled as "Fetch yahoo" and assign
action; in this case is to invoke Offlineimap with *yahoo* account,
just like you would say in command line: *offlineimap -a yahoo*.

Action can be either Scheme string or function. In case of string,
*ono* will assume you have Offlineimap account with the same name or
will not be able to run Offlineimap successfully. However, if you
would like more advanced control, you should use a function.

In our example, "Fetch All" entry is using a function with explicit
call of builtin *(offlineimap)* Scheme function. *(offlineimap)*
accepts two parameters, account entry in offlineimaprc and
Offlineimap additional arguments.

If you put **#f** (false in Scheme), *ono* will call Offlineimap
without account and arguments, which will at the end request from
Offlineimap to synchronize all accounts.

**ono-on-tray-click** tells *ono* what to do when you click on *ono*
icon in system tray (usually with left mouse button). In our example,
it will run *mrxvt* terminal in fullscreen and start Mutt mail agent.

## Command line arguments

You can run *ono -h* to see command line arguments, which should be
self-explanatory. 

## Using ono REPL

*ono* comes with builtin Scheme
[REPL](http://en.wikipedia.org/wiki/Read\u2013eval\u2013print_loop)
where you can write and evaluate Scheme code, experiment with *ono*
functions, test configuration and more. To start it, use **ono -r**
and to see how things works you can, for example, call manually
*(offlineimap)* like:

```
Ono REPL. Type (quit) to exit interpreter.
> (offlineimap "gmail" "--info")
```

which will output in REPL details about given account. This command is
the same as you would say to Offlineimap:

```
$ offlineimap -a gmail --info
```

## License

Copyright (c) Sanel Zukan. GPL v2.
