#!/usr/bin/env ruby
# frozen_string_literal: true
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

def hash s
   s.each_codepoint.reduce do |res, ch| (res * 101 + ch) & 0x7FFFFFFF end
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
IRLITH = "d"
IRDTAP = "e"
TARGET = "f"
WARN   = "g"
LFLAGS = "h"
CFLAGS = "i"
HASH   = "j"
STA    = "k"
TYPE   = "l"
DLITH  = "m"
DDTAP  = "n"
INITSC = "o"
NUMOUT = "p"
MFLAGS = "q"

UPGCCI = %W"$#{SRC}/p_upgrinfo.c"
UPGCIN = %W"$#{HDR}/u_names.h".push(*UPGCCI, "$#{HDR}/u_func.h")
WEPCCI = %W"$#{SRC}/p_weaponinfo.c"
WEPCIN = %W"$#{HDR}/p_weapons.h".push(*WEPCCI)
MONCIN = %W"$#{HDR}/w_moninfo.h"
DECOIN = %W"$#{HDR}/p_weapons.h $#{HDR}/p_data.h $#{HDR}/w_data.h
            $#{HDR}/u_names.h $#{HDR}/w_scorenums.h"
TEXTIN = Dir["text/*.txt"].map do |s| File.basename s end
HSFSIN = %w"pk7/language.gfx.txt:pk7/:lgfx
            pk7_dt/language.gfx.txt:pk7_dt/:dtgfx"
DEPS_I = [*UPGCIN, *WEPCIN, *MONCIN]
DEPS_H = Dir["source/Headers/*"].map do |s| "$#{HDR}/#{File.basename s}" end
DEPS = [*DEPS_I, *DEPS_H].uniq
SRCS = [*Dir["source/Main/*"], *UPGCCI, *WEPCCI].map do |s| File.basename s end.uniq

fp = open "build.ninja", "wb"

fp << <<_end_
#{SRC   } = source/Main
#{HDR   } = source/Headers
#{IR    } = bin
#{IRLITH} = $#{IR}/lithium
#{IRDTAP} = $#{IR}/doubletap
#{TARGET} = --target-engine=ZDoom
#{WARN  } = --warn-all --no-warn-parentheses
#{LFLAGS} = $#{TARGET} --bc-zdacs-init-delay #{LD_FLAGS}
#{CFLAGS} = $#{TARGET} $#{WARN} -i$#{HDR} --alloc-Aut 4096 #{CC_FLAGS}
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
 command = tools/hashfs.rb #{HSFSIN.join " "}
 description = HashFS
rule text
 command = cd text; ../tools/compilefs.rb #{TEXTIN.join " "}
 description = CompileFS
rule dec
 command = tools/decompat.rb $in
 description = DeCompat
rule font
 command = tools/mkfont.rb
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

build fs: fs | tools/hashfs.rb
build text: text | tools/compilefs.rb
build dec: dec #{DECOIN.join " "} | tools/decompat.rb
build font: font

build #{WEPCIN.join " "}: wepc source/Weapons.txt | tools/wepc.rb
build #{UPGCIN.join " "}: upgc source/Upgrades.txt | tools/upgc.rb
build #{MONCIN.join " "}: monc source/Monsters.txt | tools/monc.rb

build $#{IR}/libc.ir: makelib
 #{TYPE} = libc
build $#{IR}/libGDCC.ir: makelib
 #{TYPE} = libGDCC
_end_

inputs_lithium = []
inputs_doubletap = []

for f in SRCS
   fp << <<~_end_
   build $#{IRLITH}/#{f}.ir: cc $#{SRC}/#{f} | #{DEPS.join " "}
    #{HASH  } = #{hash f}
    #{CFLAGS} = $#{CFLAGS} $#{DLITH}
   build $#{IRDTAP}/#{f}.ir: cc $#{SRC}/#{f} | #{DEPS.join " "}
    #{HASH  } = #{hash f}
    #{CFLAGS} = $#{CFLAGS} $#{DDTAP}
   _end_
   inputs_lithium   << "$#{IRLITH}/#{f}.ir"
   inputs_doubletap << "$#{IRDTAP}/#{f}.ir"
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

build lithium: phony dec text fs pk7/acs/lithmain.bin pk7/acs/lithlib.bin
build doubletap: phony dec text fs pk7_dt/acs/dtmain.bin pk7_dt/acs/dtlib.bin

default lithium
_end_

## EOF
