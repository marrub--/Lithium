## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

i = txt "Trie.yaml"
o = bin "m_trie.c"
c = src "m_trie.c"
h = hdr "m_trie.h"

@ctx.fp << <<ninja
rule trie
 command = gdcc-cpp -o - #{CPP_ARG} $in | #{tool "trie.rb"} $out
 description = [trie.rb] Trie Generator
build #{h} #{c}: trie #{i} | #{tool "trie.rb"} #{hdr "u_names.h"}
ninja

@ctx.srcs.push SrcsEnt.new o, c
@ctx.ordr.push h

## EOF
