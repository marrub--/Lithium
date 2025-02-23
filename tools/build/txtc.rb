## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

i = txt "Text.txt"
o = "pk7/language.txt"

@ctx.fp << <<ninja
rule txtc
 command = #{tool "txtc.rb"} $in $out
 description = [txtc.rb] Text Compiler
build #{o}: txtc #{i} | fakein #{tool "txtc.rb"}
ninja
@ctx.fake.push o

## EOF
