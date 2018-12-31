![Lithium](http://mab.greyserv.net/f/Lithium_logo.png)

# Overview

A mod in which you shoot things until they die. See the ZDoom forums thread linked above for more details.

# Running

If you wish to run an in-development version of the mod, you can click the "Clone or download" button and then "Download ZIP".

Once you've done that you can then load the `pk7` folder into ZDoom as you would any other mod.

# Code

The C code in `source/` is compiled to ACS bytecode with [GDCC](https://github.com/DavidPH/GDCC). It can be rather terse at times, and is scarcely commented, but it is hopefully understandable on its own. Please note that it is currently not under a free software license. You may use it as reference but do not copy code from the project without asking first.

To compile, generate a build file with `tools/genbuild.rb` and run `ninja`.

# Licensing

All of the content and source code in this project is copyright © 2016-2019 Alison Sanderson unless otherwise stated. Respect content creators and do not use it elsewhere without permission. See `pk7/credits.txt` for more information.

All DECORATE code in the `pk7/lscripts` folder is public domain as defined by [the CC0 license](http://creativecommons.org/publicdomain/zero/1.0/).
