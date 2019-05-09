#!/usr/bin/env sh
## Distributed under the CC0 public domain license.
## By Alison Sanderson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
tools/genbuild.rb -DNDEBUG
ninja -t clean
ninja

args='-x *.gitignore *.bat *.dbs *.wad.b* *.swp'
(cd pk7 && zip -r ../Lithium.pk3 ./* ../licenses ../credits.txt ../bin/lithmain_ld.txt $args)

tools/genbuild.rb
