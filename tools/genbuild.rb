#!/usr/bin/env ruby
## Copyright © 2018 Alison Sanderson, all rights reserved.
## GenBuild: Generates a Ninja build file for the project.

require 'set'

def hash s
   res = 0
   s.each_codepoint{|ch| res = res * 101 + ch; res &= 0x7FFFFFFF}
   res
end

`rm -f build.ninja .ninja_deps .ninja_log`

UPGCIN = "$hdr/lith_upgradenames.h $src/p_upgrinfo.c $hdr/lith_upgradefuncs.h"
WEPCIN = "$hdr/lith_weapons.h $src/p_weaponinfo.c"
MONCIN = "$hdr/lith_monsterinfo.h"
TEXTIN = Dir["filedata/*.txt"].to_a.each{|s| s.gsub! "filedata/", ""}.join(?\s)
FSIN   = "pk7/language.gfx.txt,pk7/,lgfx pk7_dt/language.gfx.txt,pk7_dt/,dtgfx"
DEPS_I = [*UPGCIN.split, *WEPCIN.split, *MONCIN.split]
DEPS_H = Dir["source/Headers/*"].to_a.each{|s| s.gsub! "source/Headers", "$hdr"}
DEPS   = Set[*DEPS_I, *DEPS_H].to_a.join(?\s)

fp = open "build.ninja", "wb"

fp << <<_end_
src = source/Main
hdr = source/Headers
ir  = ir
target = --bc-target=ZDoom
warn   = --warn-all --no-warn-parentheses
lflags = $target --bc-zdacs-init-delay
cflags = $target $warn -i$hdr --alloc-Aut 4096

rule cc
   command = gdcc-cc $cflags -DFileHash=$hash -c $in -o $out
   description = CC $out
rule makelib
   command = gdcc-makelib $target -c $type -o $out
   description = MakeLib $out
rule ld
   command = gdcc-ld $lflags --alloc-min Sta "" $sta $in -o $out
   description = LD $out
rule fs
   command = tools/hashfs.rb #{FSIN}
   description = HashFS
rule text
   command = cd filedata; ../tools/compilefs.rb #{TEXTIN}
   description = CompileFS
rule dec
   command = tools/decompat.rb $in
   description = DeCompat
rule gettf
   command = tools/gettf.sh
   description = Getting ttfuck
rule font
   command = rm -f pk7/lgfx/Font/*/*.png; tools/mkfont.rb
   description = Font (remember to run PNGGauntlet!)
rule wepc
   command = tools/wepc.rb $in $out
   description = WepC
rule upgc
   command = tools/upgc.rb $in $out
   description = UpgC
rule monc
   command = tools/monc.rb $in $out
   description = MonC

build tools/ttfuck/ttfuck.exe: gettf
build fs: fs | tools/hashfs.rb
build text: text | tools/compilefs.rb
build dec: dec $hdr/lith_weapons.h $hdr/lith_pdata.h $hdr/lith_wdata.h $hdr/lith_upgradenames.h $hdr/lith_scorenums.h | tools/decompat.rb
build font: font | tools/ttfuck/ttfuck.exe
build #{WEPCIN}: wepc source/Weapons.txt | tools/wepc.rb
build #{UPGCIN}: upgc source/Upgrades.txt | tools/upgc.rb
build #{MONCIN}: monc source/Monsters.txt | tools/monc.rb
build $ir/libc.ir: makelib
   type = libc
build $ir/libGDCC.ir: makelib
   type = libGDCC
_end_

inputs_lithium = []
inputs_doubletap = []

for f in Dir["source/Main/*"]
   f.replace File.basename f
   fp << <<~_end_
   build $ir/lithium/#{f}.ir: cc $src/#{f} | #{DEPS}
      hash = #{hash f}
      cflags = $cflags -DLITHIUM=1
   build $ir/doubletap/#{f}.ir: cc $src/#{f} | #{DEPS}
      hash = #{hash f}
      cflags = $cflags -DDOUBLETAP=1
   _end_
   inputs_lithium   << "$ir/lithium/#{f}.ir"
   inputs_doubletap << "$ir/doubletap/#{f}.ir"
end

fp << <<_end_
build pk7/acs/lithmain.bin: ld #{inputs_lithium.join ?\s}
   lflags = $lflags -llithlib --bc-zdacs-init-script-name "__lithmain.bin_init"
   sta = 1400000
build pk7/acs/lithlib.bin: ld $ir/libc.ir $ir/libGDCC.ir
   lflags = $lflags --bc-zdacs-init-script-name "__lithlib.bin_init"
   sta = 70000
build pk7_dt/acs/dtmain.bin: ld #{inputs_doubletap.join ?\s}
   lflags = $lflags -ldtlib --bc-zdacs-init-script-name "__dtmain.bin_init"
   sta = 1400000
build pk7_dt/acs/dtlib.bin: ld $ir/libc.ir $ir/libGDCC.ir
   lflags = $lflags --bc-zdacs-init-script-name "__dtlib.bin_init"
   sta = 70000
build doubletap: phony dec text fs pk7_dt/acs/dtmain.bin pk7_dt/acs/dtlib.bin
build lithium: phony dec text fs pk7/acs/lithmain.bin pk7/acs/lithlib.bin

default lithium
_end_

## EOF
