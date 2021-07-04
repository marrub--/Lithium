# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison G. Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|

hsfs = [
   {o:     "pk7/language.gfx.txt", p:     "pk7/", d: "lgfx"},
   {o: "pk7_ep1/language.gfx.txt", p: "pk7_ep1/", d: "lgfx"},
]
@ctx.fp << <<ninja
rule hsfs
 command = #{tool "hashfs.rb"} $out $in $#{DIR}
 description = [hashfs.rb] Logical Graphic Name Generator
ninja

for ent in hsfs
   @ctx.fp << <<ninja
build #{ent[:o]}: hsfs | fakein #{tool "hashfs.rb"}
 #{DIR} = #{ent[:p]} #{ent[:d]}
ninja
   @ctx.fake.push ent[:o]
end

## EOF
