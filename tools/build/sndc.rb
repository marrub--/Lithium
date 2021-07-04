# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison G. Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|

i = txt "Sounds.txt"
o = "pk7/sndinfo.txt"
@ctx.fp << <<ninja
rule sndc
 command = #{tool "sndc.rb"} $in $out
 description = [sndc.rb] Sound Compiler
build #{o}: sndc #{i} | fakein #{tool "sndc.rb"}
ninja
@ctx.fake.push o

## EOF
