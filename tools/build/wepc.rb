# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison G. Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|

i = txt "Weapons.yaml"
o = bin "p_weaponinfo.c"
c = src "p_weaponinfo.c"
h = hdr "p_weaponinfo.h"

@ctx.fp << <<ninja
rule wepc
 command = #{tool "wepc.rb"} $in $out
 description = [wepc.rb] Weapon Compiler
build #{h} #{c}: wepc #{i} | #{tool "wepc.rb"}
ninja

@ctx.srcs.push SrcsEnt.new o, c
@ctx.ordr.push h

## EOF
