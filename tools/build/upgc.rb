# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison G. Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|

i = txt "Upgrades.yaml"
o = bin "p_upgrinfo.c"
c = src "p_upgrinfo.c"
h = hdr "u_names.h"
f = hdr "u_func.h"

@ctx.fp << <<ninja
rule upgc
 command = #{tool "upgc.rb"} $in $out
 description = [upgc.rb] Upgrade Compiler
build #{h} #{c} #{f}: upgc #{i} | #{tool "upgc.rb"}
ninja

@ctx.srcs.push SrcsEnt.new o, c
@ctx.ordr.push h, f

## EOF
