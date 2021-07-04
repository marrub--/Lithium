# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison G. Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|

check_uniq "link dependencies", @ctx.link

@ctx.fp << <<ninja
rule ld
 command = gdcc-ld #{TARGET} --bc-opt #{LD_ARG} --alloc-min Sta "" 70000 --no-bc-zdacs-init-script-named --bc-zdacs-init-script-number 17000 $in -o $out --bc-zdacs-dump-ScriptI #{bin "lithmain_ld.txt"}
 description = [gdcc-ld] Link $out
build pk7/acs/lithmain.bin | #{bin "lithmain_ld.txt"}: ld #{@ctx.link.join " "}
default pk7/acs/lithmain.bin
ninja

## EOF
