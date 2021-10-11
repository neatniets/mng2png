mng2png
=======
**mng2png** - convert MNG to PNG(s).

This was made because `convert` kept freezing my computer when I tried to
convert MNG files to PNGs.
This seemed easier than trying to figure out the issue with `convert`.

Usage
=====
Each frame of the MNG video is converted into a PNG using 32-bit RGBA, which is
color type 6 and bit depth 8.
Note that endlessly-looping MNG files are NOT detected and will continue to
write PNG files.

The PNG files are written in the directory that the command is executed from.
They will be labeled as *number*.png, where *number* is the MNG frame number
starting from 0.

Further usage details are left to the manpage, `mng2png.1`.

Installation
============
Requirements
------------
- A C99-compliant compiler.
- `libmng` compiled with support for reading and displaying.
	- Tested with version 2.0.3.
- `libpng` compiled with support for writing.
	- `setjmp()` is used if support is compiled for it.
	- Tested with version 1.6.37.

The dependencies are not bundled with this source.

Building
--------
`make all`

A `Makefile` is supplied to automatically build, if `make` is supported on your
system.
`config.mk` allows you to configure system-dependent information, such as
compiler and compiler flags.
The default configuration uses `gcc`, includes debugging symbols, no
optimizations, and tells the linker to find the dependencies as system
libraries.

If `make` is unavailable, simply compile the source with a C compiler and link
against the dependencies.

Installing
----------
`make install`

The `Makefile` provides an `install` rule for Unix-like systems, with
`config.mk` having definitions for installation directories of the program and
manpage.

Consult your system for installation-specifics.

TODO
====
See `TODO` messages left in source for various future considerations.
