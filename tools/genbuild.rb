#!/usr/bin/env ruby
## Copyright © 2018 Alison Sanderson, all rights reserved.
## GenBuild: Generates a Ninja build file for the project.

def hash s
   res = 0
   s.each_codepoint do |ch| res = res * 101 + ch; res &= 0x7FFFFFFF end
   res
end

`rm -f build.ninja .ninja_deps .ninja_log`

upgcin = "$hdr/lith_upgradenames.h $src/p_upgrinfo.c $hdr/lith_upgradefuncs.h"
wepcin = "$hdr/lith_weapons.h $src/p_weaponinfo.c"

deps = "#{upgcin} #{wepcin} #{Dir["source/Headers/*"].to_a.map{|s| s.gsub "source/Headers", "$hdr"}.join(" ")}"

fp = open "build.ninja", "wb"

fp << <<NINJA
src    = source/Main
hdr    = source/Headers
bin    = pk7/acs
ir     = ir
target = --bc-target=ZDoom
warn   = --warn-all --no-warn-parentheses
lflags = $target --bc-zdacs-init-delay --bc-zdacs-chunk-STRE
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
   command = tools/hashfs.rb
   description = HashFS
rule text
   command = cd filedata; ../tools/compilefs.rb _Directory.txt
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

build tools/ttfuck/ttfuck.exe: gettf

build fs: fs | tools/hashfs.rb

build text: text | tools/compilefs.rb

build dec: dec $hdr/lith_weapons.h $hdr/lith_pdata.h $hdr/lith_wdata.h $hdr/lith_upgradenames.h $hdr/lith_scorenums.h | tools/decompat.rb

build font: font | tools/ttfuck/ttfuck.exe

build #{wepcin}: wepc source/Weapons.txt | tools/wepc.rb

build #{upgcin}: upgc source/Upgrades.txt | tools/upgc.rb

build $ir/libc.ir: makelib
   type = libc
build $ir/libGDCC.ir: makelib
   type = libGDCC

NINJA

inputs = []

Dir["source/Main/*"].each do |f|
   f = File.basename f
   fp << <<~NINJA
      build $ir/#{f}.ir: cc $src/#{f} | #{deps}
         hash = #{hash f}
   NINJA
   inputs << "$ir/#{f}.ir"
end

fp << <<NINJA

build $bin/lithmain.bin: ld #{inputs.join " "}
   lflags = $lflags -llithlib --bc-zdacs-init-script-name "__lithmain.bin_init"
   sta = 1400000

build $bin/lithlib.bin: ld $ir/libc.ir $ir/libGDCC.ir
   lflags = $lflags --bc-zdacs-init-script-name "__lithlib.bin_init"
   sta = 70000

default dec text fs $bin/lithmain.bin $bin/lithlib.bin

## EOF
NINJA

## EOF
