#!/usr/bin/env sh
## Distributed under the CC0 public domain license.
## By Alison Sanderson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
args='-xr!*.gitignore -xr!*.bat -xr!*.dbs -xr!*.wad.b* -tzip'
(cd pk7 && 7za a ../Lithium.pk3 ./* ../licenses ../credits.txt $args)
