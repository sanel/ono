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

## Installation

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
language easy to learn and there is numerous tutorials online for
beginners. For example
[Yet Another Scheme Tutorial](http://www.shido.info/lisp/idx_scm_e.html)
is really nice introduction to Scheme.


## License

Copyright (c) Sanel Zukan. GPL v2.
