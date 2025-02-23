#!/usr/bin/env fish
## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ├──────────────────────────────────────────────────────────────────────────┤
## │                                                                          │
## │ Script to generate master packages of Lithium.                           │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

function build
   if test ! $DEBUG
      set -xa CC_ARG "-DNDEBUG"
   end

   rm -rf master Lithium.pk3

   ninja -t clean
   tools/genbuild.rb
   ninja

   mkdir master
   pushd master
   cp -r --archive ../pk7/*    .
   cp -r --archive ../text     ltext
   cp -r --archive ../source   lsource
   cp -r --archive ../tools    ltools
   cp -r --archive ../licenses licenses
   cp    --archive ../credits.txt .
   cp    --archive ../bin/lithmain_ld.txt.ir .
   zip -r ../Lithium.pk3 ./* \
       -x '*.gitignore' '*.bat' '*.dbs' '*.wad.b*' '*.swp'
   popd

   ninja -t clean
end

build 2>&1 > /tmp/lbuild

set -xa CC_ARG
tools/genbuild.rb
