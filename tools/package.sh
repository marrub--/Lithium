#!/usr/bin/env sh
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

if [ $RELEASE ]
   CC_FLAGS=$CC_FLAGS -DRELEASE
fi

if [ ! $DEBUG ]
   CC_FLAGS=$CC_FLAGS -DNDEBUG
fi

(rm -rf master Lithium.pk3 &&

 env CC_FLAGS=$CC_FLAGS tools/genbuild.rb &&
 ninja -t clean &&
 ninja &&

 mkdir master &&
 cd master &&
 cp -r --archive ../pk7/*    . &&
 cp -r --archive ../text     ltext &&
 cp -r --archive ../source   lsource &&
 cp -r --archive ../tools    ltools &&
 cp -r --archive ../licenses licenses &&
 cp    --archive ../credits.txt . &&
 cp    --archive ../bin/lithmain_ld.txt . &&
 zip -r ../Lithium.pk3 ./* \
     -x '*.gitignore' '*.bat' '*.dbs' '*.wad.b*' '*.swp') 2>&1 > /tmp/lbuild

tools/genbuild.rb
