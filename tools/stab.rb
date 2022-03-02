#!/usr/bin/env -S ruby --enable=frozen-string-literal -w
## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Alison G. Watson. Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ├──────────────────────────────────────────────────────────────────────────┤
## │                                                                          │
## │ Generates the stab file.                                                 │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

require_relative "corinth.rb"
require "yaml"

common_main do
   hsh = YAML.load STDIN.read

   strs = []
   arys = []

   for name_pfx, prefixes in hsh
      if name_pfx == "ARRAYS"
         for k, vs in prefixes
            arys << ["sa_" + k, vs]
         end
         next
      end
      for text_pfx, keys in prefixes
         for key in keys
            if key.is_a? String
               k = key
               v = key
            else
               k = key.keys.first
               v = key[k]
            end
            strs << [name_pfx + k, text_pfx + v]
         end
      end
   end

   open(ARGV.shift, "wt").puts <<_end_h_
#{generated_header "stab"}

#if defined(stab_x)

#{
res = String.new
for str in strs
   res.concat "stab_x("
   res.concat str[0]
   res.concat ", \""
   res.concat str[1]
   res.concat "\")\n"
end
for ary in arys
   res.concat "stab_ary_bgn_x("
   res.concat ary[0]
   res.concat ")\n"
   for str in ary[1]
   res.concat "stab_ary_ent_x(\""
   res.concat str
   res.concat "\")\n"
   end
   res.concat "stab_ary_end_x()\n"
end
res
}
#undef stab_ary_bgn_x
#undef stab_ary_ent_x
#undef stab_ary_end_x
#undef stab_x
#elif !defined(m_stab_h)
#define m_stab_h

#define stab_ary_bgn_x(name) extern str name[0
#define stab_ary_ent_x(strn) + 1
#define stab_ary_end_x()     ];
#define stab_x(n, s) extern str n;
#include "m_stab.h"

#endif

/* EOF */
_end_h_
end

## EOF
