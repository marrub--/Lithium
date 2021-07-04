# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison G. Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|

o = bin "libc"

@ctx.fp << <<ninja
rule makelib
 command = gdcc-makelib #{TARGET} #{MK_ARG} -c libc -o $out
 description = [gdcc-makelib] Compile Library $out
build #{o}: makelib
ninja

@ctx.link.push o

## EOF
