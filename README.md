# ono 

Small application that sits inside system tray, allowing you to
manage [Offlineimap](http://offlineimap.org) and mail agent easily.

<p align="center">
<img src="https://raw.github.com/sanel/ono/master/images/shot.png"
     alt="ono" title="ono screenshot">
</p>

## Introduction

*ono* is free, configurable and portable UNIX GUI application from
where you can easily handle Offlineimap and start any mail agent
(including terminal agents, like [Mutt](http://www.mutt.org)).

Thanks to builtin scripting language, you can even abuse *ono* to
serve any program that doesn't support system tray integration.

## Installation

Make sure you have C compiler, GNU make and Gtk+2 (compilation under
Gtk+3 isn't tested, but should work out of the box) installed.

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

## License

Copyright (c) Sanel Zukan. GPL v2.
