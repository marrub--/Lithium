## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Alison G. Watson. Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

@ctx.fp << <<ninja
rule genbuild
 command = #{tool "genbuild.rb"}
 generator = 1
 description = [genbuild.rb] Generate Build
build build.ninja: genbuild | #{@ctx.rgen.join " "}
ninja

## EOF
