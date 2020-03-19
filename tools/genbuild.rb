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

require_relative "corinth.rb"

def hash s
   s.each_codepoint.reduce do |res, ch| (res * 101 + ch) & 0x7FFFFFFF end
end

LIB_PATH      = (ENV["LIB_PATH"] ? "--lib-path='#{ENV["LIB_PATH"]}'" : "") + " "
ALL_FLAGS     = (ENV["ALL_FLAGS"] || "") + " "
MAKELIB_FLAGS = (ENV["MAKELIB_FLAGS"] || "") + " " + ALL_FLAGS + LIB_PATH
CC_FLAGS      = (ENV["CC_FLAGS"]      || "") + " " + ALL_FLAGS + LIB_PATH
LD_FLAGS      = (ENV["LD_FLAGS"]      || "") + " " + ALL_FLAGS

`rm -f build.ninja .ninja_deps .ninja_log`

SRC    = "a"
HDR    = "b"
IR     = "c"
IRLITH = "d"
TARGET = "f"
WARN   = "g"
LFLAGS = "h"
CFLAGS = "i"
HASH   = "j"
STA    = "k"
TYPE   = "l"
DLITH  = "m"
INITSC = "o"
NUMOUT = "p"
MFLAGS = "q"
TXT    = "r"

UPGCIN = %W"$#{TXT}/Upgrades.txt"
UPGCCO = %W"$#{SRC}/p_upgrinfo.c"
UPGCHO = %W"$#{HDR}/u_names.h".push(*UPGCCO, "$#{HDR}/u_func.h")

WEPCIN = %W"$#{TXT}/Weapons.txt"
WEPCCO = %W"$#{SRC}/p_weaponinfo.c"
WEPCHO = %W"$#{HDR}/p_weapons.h".push(*WEPCCO)

MONCIN = %W"$#{TXT}/Monsters.txt"
MONCHO = %W"$#{HDR}/w_moninfo.h"

DECOIN = %W"
   $#{HDR}/m_drawing.h
   $#{HDR}/p_data.h
   $#{HDR}/p_weapons.h
   $#{HDR}/u_names.h
   $#{HDR}/w_data.h
   $#{HDR}/w_monster.h
   $#{HDR}/w_scorenums.h
"

TEXTIN = %W"$#{TXT}/Text.txt"

HSFSIN = %W"pk7/language.gfx.txt:pk7/:lgfx
            pk7_ep1/language.gfx.txt:pk7_ep1/:lgfx"

SNDSIN = %W"$#{TXT}/Sounds.txt"

DEPS = [*UPGCHO, *WEPCHO, *MONCHO,
        Dir.glob("source/include/*").map do |s|
           "$#{HDR}/#{File.basename s}"
        end].uniq

SRCS = [*Dir.glob("source/*.c"), *UPGCCO, *WEPCCO].map do |s|
   File.basename s
end.uniq

fp = open "build.ninja", "wb"

fp << <<_end_
#{SRC   } = source
#{HDR   } = source/include
#{TXT   } = text
#{IR    } = bin
#{IRLITH} = $#{IR}/lithium
#{TARGET} = --target-engine=ZDoom --target-format=ACSE --func-minimum ScriptI 17000
#{WARN  } = --warn-all --no-warn-parentheses
#{LFLAGS} = $#{TARGET} --bc-opt --bc-zdacs-init-delay #{LD_FLAGS}
#{CFLAGS} = $#{TARGET} $#{WARN} -i$#{HDR} --alloc-Aut 4096 #{CC_FLAGS}
#{MFLAGS} = $#{TARGET} #{MAKELIB_FLAGS}
#{INITSC} = --bc-zdacs-init-script-name

rule cc
 command = gdcc-cc $#{CFLAGS} -DFileHash=$#{HASH} -c $in -o $out
 description = CC $out
rule makelib
 command = gdcc-makelib $#{MFLAGS} -c $#{TYPE} -o $out
 description = MakeLib $out
rule ld
 command = gdcc-ld $#{LFLAGS} --alloc-min Sta "" $#{STA} $in -o $out --bc-zdacs-dump-ScriptI $#{NUMOUT}
 description = LD $out
rule fs
 command = tools/hashfs.rb #{HSFSIN.join " "}
 description = HashFS
rule text
 command = tools/compilefs.rb $in
 description = CompileFS
rule snd
 command = tools/compilesnd.rb $in
 description = CompileSnd
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

build _fs: fs | tools/hashfs.rb
build _text: text #{TEXTIN.join " "} | tools/compilefs.rb
build _snd: snd #{SNDSIN.join " "} | tools/compilesnd.rb
build _dec: dec #{DECOIN.join " "} | tools/decompat.rb
build _font: font

build #{WEPCHO.join " "}: wepc #{WEPCIN.join " "} | tools/wepc.rb
build #{UPGCHO.join " "}: upgc #{UPGCIN.join " "} | tools/upgc.rb
build #{MONCHO.join " "}: monc #{MONCIN.join " "} | tools/monc.rb

build $#{IR}/libc.ir: makelib
 #{TYPE} = libc
build $#{IR}/libGDCC.ir: makelib
 #{TYPE} = libGDCC
_end_

inputs_lithium = []

for f in SRCS
   fp << <<~_end_
   build $#{IRLITH}/#{f}.ir: cc $#{SRC}/#{f} | #{DEPS.join " "}
    #{HASH  } = #{hash f}
    #{CFLAGS} = $#{CFLAGS}
   _end_
   inputs_lithium << "$#{IRLITH}/#{f}.ir"
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

build lithium: phony _dec _snd _text _fs pk7/acs/lithmain.bin pk7/acs/lithlib.bin

default lithium
_end_

## EOF
