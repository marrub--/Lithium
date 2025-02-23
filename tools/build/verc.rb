## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

i = txt "Versions.yaml"
h = hdr "m_version.h"

@ctx.fp << <<ninja
rule verc
 command = #{tool "verc.rb"} $in $out
 description = [verc.rb] Version Compiler
build #{h}: verc #{i} | #{tool "verc.rb"}
ninja

@ctx.ordr.push h

## EOF
