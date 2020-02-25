#!/usr/bin/env sh
## Distributed under the CC0 public domain license.
## By Alison Sanderson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
rm -rf tools/ttfuck
mkdir tools/ttfuck
wget http://mab.greyserv.net/f/ttfuck.7z
7za x ttfuck.7z -otools/ttfuck
rm ttfuck.7z
chmod +x tools/ttfuck/ttfuck.exe
