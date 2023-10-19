#!/usr/bin/env -S ruby --enable=frozen-string-literal -w
## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Alison G. Watson. Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ├──────────────────────────────────────────────────────────────────────────┤
## │                                                                          │
## │ Version info compiler.                                                   │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

require_relative "corinth.rb"
require "yaml"

common_main do
   hsh = YAML.load(IO.read(ARGV.shift), symbolize_names: true)
   vers = hsh[:all].map do |ver|
      ver = {
         maj: ver[0],
         min: ver[1],
         pat: ver[2],
         bui: ver[3],
         bna: ver[4],
         nam: ver[5],
      }

      def ver.ver_num
         self[:maj] * 1000000 +
         self[:min] *   10000 +
         self[:pat] *     100 +
         self[:bui]
      end

      def ver.ver_str
         self[:maj].to_s + "." +
         self[:min].to_s + "." +
         self[:pat].to_s +
         self[:bna]
      end

      def ver.ver_nam
         ver_str + " (" + self[:nam] + ")"
      end

      ver
   end
   src = String.new

   src.concat <<~_end_c_
#{generated_header "verc"}

#if defined(VERSION)
#{
   vers.reduce(String.new) do |s, ver|
      s.concat "VERSION(#{ver[:maj]}, #{ver[:min]}, #{ver[:pat]}, #{ver[:bui]}, \"#{ver[:bna]}\", \"#{ver[:nam]}\")\n"
   end
}
#undef VERSION
#elif defined(LEGACYVERSION)
#{
   hsh[:legacy].keys.reduce(String.new) do |s, k|
      s.concat "LEGACYVERSION(#{k}, vernum_#{hsh[:legacy][k]})\n"
   end
}
#undef LEGACYVERSION
#else
#{
   vers.reduce(String.new) do |s, ver|
      s.concat "#define vernum_#{ver[:maj]}_#{ver[:min]}_#{ver[:pat]}_#{ver[:bui]} #{ver.ver_num}\n"
      s.concat "#define verstr_#{ver[:maj]}_#{ver[:min]}_#{ver[:pat]}_#{ver[:bui]} \"#{ver.ver_str}\"\n"
      s.concat "#define vernam_#{ver[:maj]}_#{ver[:min]}_#{ver[:pat]}_#{ver[:bui]} \"#{ver.ver_nam}\"\n"
   end
}
#define vernum vernum_#{hsh[:current]}
#define verstr verstr_#{hsh[:current]}
#define vernam vernam_#{hsh[:current]}
#if defined(ZscOn)
const LITH_VERNAM = vernam;
const LITH_VERNUM = vernum;
const LITH_VERSTR = verstr;
const LITH_VERSION = vernam
#ifndef NDEBUG
" ("
#ifdef RELEASE
"DEBUG"
#else
"DEV"
#endif
")"
#endif
;
#elif !defined(VER_DEFINES_ONLY)
i32 version_name_to_num(cstr s);
#endif
#undef VER_DEFINES_ONLY
#endif

/* EOF */
   _end_c_

   open(ARGV.shift, "wt").puts src
end

## EOF
