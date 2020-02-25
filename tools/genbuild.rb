#!/usr/bin/env ruby
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison Sanderson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Generates a Ninja build file for the project.
##
## ---------------------------------------------------------------------------|

require 'set'

def hash s
   res = 0
   s.each_codepoint{|ch| res = res * 101 + ch; res &= 0x7FFFFFFF}
   res
end

LIB_PATH      = (ENV["LIB_PATH"] ? "--lib-path='#{ENV["LIB_PATH"]}'" : "")
ALL_FLAGS     = ENV["ALL_FLAGS"] || ""
MAKELIB_FLAGS = (ENV["MAKELIB_FLAGS"] || "") + ALL_FLAGS + LIB_PATH
CC_FLAGS      = (ENV["CC_FLAGS"]      || "") + ALL_FLAGS + LIB_PATH
LD_FLAGS      = (ENV["LD_FLAGS"]      || "") + ALL_FLAGS

`rm -f build.ninja .ninja_deps .ninja_log`

SRC    = "a"
HDR    = "b"
IR     = "c"
TARGET = "d"
WARN   = "e"
LFLAGS = "f"
CFLAGS = "g"
HASH   = "h"
STA    = "i"
TYPE   = "j"
DLITH  = "k"
DDTAP  = "l"
INITSC = "m"
NUMOUT = "n"
MFLAGS = "o"

UPGCIN = "$#{HDR}/u_names.h $#{SRC}/p_upgrinfo.c $#{HDR}/u_func.h"
WEPCIN = "$#{HDR}/p_weapons.h $#{SRC}/p_weaponinfo.c"
MONCIN = "$#{HDR}/w_moninfo.h"
DECOIN = "$#{HDR}/p_weapons.h $#{HDR}/p_data.h $#{HDR}/w_data.h " \
         "$#{HDR}/u_names.h $#{HDR}/w_scorenums.h"
TEXTIN = Dir["filedata/*.txt"].to_a.each{|s| s.gsub! "filedata/", ""}.join(" ")
HSFSIN = "pk7/language.gfx.txt:pk7/:lgfx pk7_dt/language.gfx.txt:pk7_dt/:dtgfx"
DEPS_I = [*UPGCIN.split, *WEPCIN.split, *MONCIN.split]
DEPS_H = Dir["source/Headers/*"].to_a.each{|s| s.gsub! "source/Headers", "$#{HDR}"}
DEPS   = Set[*DEPS_I, *DEPS_H].to_a.join(" ")

fp = open "build.ninja", "wb"

fp << <<_end_
#{SRC   } = source/Main
#{HDR   } = source/Headers
#{IR    } = bin
#{TARGET} = --target-engine=ZDoom
#{WARN  } = --warn-all --no-warn-parentheses
#{LFLAGS} = $#{TARGET} --bc-zdacs-init-delay #{LD_FLAGS}
#{CFLAGS} = $#{TARGET} $#{WARN} -i$#{HDR} --alloc-Aut 4096 #{ARGV.join " "} #{CC_FLAGS}
#{MFLAGS} = $#{TARGET} #{MAKELIB_FLAGS}
#{DLITH } = -DLITHIUM=1
#{DDTAP } = -DDOUBLETAP=1
#{INITSC} = --bc-zdacs-init-script-name

rule cc
 command = gdcc-cc $#{CFLAGS} -DFileHash=$#{HASH} -c $in -o $out
 description = CC $out
rule makelib
 command = gdcc-makelib $#{MFLAGS} -c $#{TYPE} -o $out
 description = MakeLib $out
rule ld
 command = gdcc-ld $#{LFLAGS} --alloc-min Sta "" $#{STA} $in -o $out --bc-zdacs-dump-ScriptI $#{NUMOUT} --func-minimum ScriptI 17000
 description = LD $out
rule fs
 command = tools/hashfs.rb #{HSFSIN}
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
 description = Font
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
build dec: dec #{DECOIN} | tools/decompat.rb
build font: font | tools/ttfuck/ttfuck.exe
build #{WEPCIN}: wepc source/Weapons.txt | tools/wepc.rb
build #{UPGCIN}: upgc source/Upgrades.txt | tools/upgc.rb
build #{MONCIN}: monc source/Monsters.txt | tools/monc.rb
build $#{IR}/libc.ir: makelib
 #{TYPE} = libc
build $#{IR}/libGDCC.ir: makelib
 #{TYPE} = libGDCC
_end_

inputs_lithium = []
inputs_doubletap = []

for f in Dir["source/Main/*"]
   f.replace File.basename f
   fp << <<~_end_
   build $#{IR}/lithium/#{f}.ir: cc $#{SRC}/#{f} | #{DEPS}
    #{HASH  } = #{hash f}
    #{CFLAGS} = $#{CFLAGS} $#{DLITH}
   build $#{IR}/doubletap/#{f}.ir: cc $#{SRC}/#{f} | #{DEPS}
    #{HASH  } = #{hash f}
    #{CFLAGS} = $#{CFLAGS} $#{DDTAP}
   _end_
   inputs_lithium   << "$#{IR}/lithium/#{f}.ir"
   inputs_doubletap << "$#{IR}/doubletap/#{f}.ir"
end

fp << <<_end_
build pk7/acs/lithmain.bin: ld #{inputs_lithium.join " "}
 #{LFLAGS} = $#{LFLAGS} -llithlib $#{INITSC} "lithmain@gsinit"
 #{STA   } = 1400000
 #{NUMOUT} = $#{IR}/lithmain_ld.txt
build pk7/acs/lithlib.bin: ld $#{IR}/libc.ir $#{IR}/libGDCC.ir
 #{LFLAGS} = $#{LFLAGS} $#{INITSC} "lithlib@gsinit"
 #{STA   } = 70000
 #{NUMOUT} = $#{IR}/lithlib_ld.txt
build pk7_dt/acs/dtmain.bin: ld #{inputs_doubletap.join " "}
 #{LFLAGS} = $#{LFLAGS} -ldtlib $#{INITSC} "dtmain@gsinit"
 #{STA   } = 1400000
 #{NUMOUT} = $#{IR}/dtmain_ld.txt
build pk7_dt/acs/dtlib.bin: ld $#{IR}/libc.ir $#{IR}/libGDCC.ir
 #{LFLAGS} = $#{LFLAGS} $#{INITSC} "dtlib@gsinit"
 #{STA   } = 70000
 #{NUMOUT} = $#{IR}/dtlib_ld.txt
build doubletap: phony dec text fs pk7_dt/acs/dtmain.bin pk7_dt/acs/dtlib.bin
build lithium: phony dec text fs pk7/acs/lithmain.bin pk7/acs/lithlib.bin

default lithium
_end_

## EOF
