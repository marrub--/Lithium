## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Alison G. Watson. Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

i = txt "Models.txt"
o = "pk7/modeldef.txt"
@ctx.fp << <<ninja
rule mdlc
 command = #{tool "mdlc.rb"} $in $out
 description = [mdlc.rb] Model Compiler
build #{o}: mdlc #{i} | fakein #{tool "mdlc.rb"}
ninja
@ctx.fake.push o

## EOF
