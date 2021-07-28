## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison G. Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|

check_uniq "sources",            @ctx.srcs
check_uniq "dependencies",       @ctx.deps
check_uniq "order-dependencies", @ctx.deps

@ctx.fp << <<ninja
rule cc
 command = gdcc-cc #{TARGET} --warn-all --no-warn-parentheses #{CPP_ARG} --alloc-Aut 8192 #{CC_ARG} -D'ZscName(x, ...)=__VA_ARGS__' -DFileHash=$fhash -c $in -o $out
 description = [gdcc-cc] Compile C $out
ninja

deps = @ctx.deps.join " "
ordr = @ctx.ordr.join " "

for ent in @ctx.srcs
   @ctx.fp << <<ninja
build #{ent.o}: cc #{ent.c} | #{deps} || #{ordr}
 fhash = #{ent.hash}
ninja

   @ctx.link.push ent.o
end

## EOF
