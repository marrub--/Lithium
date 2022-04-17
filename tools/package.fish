#!/usr/bin/env fish
## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Alison G. Watson. Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ├──────────────────────────────────────────────────────────────────────────┤
## │                                                                          │
## │ Script to generate master packages of Lithium.                           │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

if test $RELEASE
   set -xa CC_ARG "-DRELEASE"
end

if test ! $DEBUG
   set -xa CC_ARG "-DNDEBUG"
end

function build
   rm -rf master Lithium.pk3

   tools/genbuild.rb
   ninja -t clean
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
end

build 2>&1 > /tmp/lbuild

tools/genbuild.rb
