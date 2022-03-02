## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Alison G. Watson. Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

i = txt "Strings.yaml"
h = hdr "m_stab.h"

@ctx.fp << <<ninja
rule stab
 command = gdcc-cpp -o - #{CPP_ARG} $in | #{tool "stab.rb"} $out
 description = [stab.rb] String Table Generator
build #{h}: stab #{i} | #{tool "stab.rb"} #{hdr "m_drawing.h"} #{hdr "u_data.h"}
ninja

@ctx.ordr.push h

## EOF
