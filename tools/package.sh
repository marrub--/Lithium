#!/usr/bin/env sh
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison Sanderson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Script to generate master packages of Lithium.
##
## ---------------------------------------------------------------------------|

(env CC_FLAGS=-DNDEBUG tools/genbuild.rb &&
 ninja -t clean &&
 ninja &&

 rm -rf master Lithium.pk3 &&

 mkdir master &&
 cd master &&
 cp -r --archive ../pk7/*    . &&
 cp -r --archive ../text     ltext &&
 cp -r --archive ../source   lsource &&
 cp -r --archive ../tools    ltools &&
 cp -r --archive ../licenses licenses &&
 cp    --archive ../credits.txt . &&
 cp    --archive ../bin/lithmain_ld.txt . &&
 zip -r ../Lithium.pk3 ./* -x '*.gitignore' '*.bat' '*.dbs' '*.wad.b*' '*.swp')

tools/genbuild.rb
