#!/usr/bin/env ruby
# frozen_string_literal: true
## ---------------------------------------------------------------------------|
##
## Distributed under the CC0 public domain license.
## By Alison G. Watson. Attribution is encouraged, though not required.
## See licenses/cc0.txt for more information.
##
## ---------------------------------------------------------------------------|
##
## Upgrade info text compiler.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

def zstr s
   s.gsub "\0", "\\\\0"
end

def generate_strhash_switch kvp, ind = 1, it = 0
   res = String.new
   itx = "   "
   res.concat "#{itx*ind}switch(#{%w"fst snd thd"[it]}) {\n"
   for key, nxt in kvp
      ind += 1
      z0 = zstr key[0]
      z1 = zstr key[1]
      z2 = zstr key[2]
      z3 = zstr key[3]
      res.concat "#{itx*ind}case FourCC('#{z0}', '#{z1}', '#{z2}', '#{z3}'):\n"
      ind += 1
      if nxt.is_a? String
         res.concat "#{itx*ind}return #{nxt};\n"
      else
         res.concat generate_strhash_switch nxt, ind, it + 1
         res.concat "#{itx*ind}break;\n"
      end
      ind -= 2
   end
   res.concat "#{itx*ind}}\n"
   res
end

def generate_strhasher keys, pfx
   sorted_keys = {}
   for key in keys
      fst = key[0... 4].to_s.ljust 4, "\0"
      snd = key[4... 8].to_s.ljust 4, "\0"
      thd = key[8...12].to_s.ljust 4, "\0"
      sorted_keys[fst]           = {}        unless sorted_keys[fst]
      sorted_keys[fst][snd]      = {}        unless sorted_keys[fst][snd]
      sorted_keys[fst][snd][thd] = pfx + key unless sorted_keys[fst][snd][thd]
   end
   generate_strhash_switch sorted_keys
end

common_main do
   tks = tokenize ARGV.shift

   upgrades = {}
   category = ""

   loop do
      tok = tks.next.expect_in_top [:colon, :plus, :eof]

      case tok.type
      when :colon
         tok = tks.next.expect_after tok, :identi
         category = "UC_" + tok.text
      when :plus
         tok = tks.next.expect_after tok, :identi
         una = tok.text

         tok = tks.next.expect_after tok, :identi
         inf = if tok.text == "N/A" then "snil" else 's"' + tok.text + '"' end

         scr = tks.peek_or :number, "0"

         pcl = []
         tks.while_drop :bar do
            tok = tks.next.expect_after tok, :identi
            pcl.push tok.text
         end
         pcl = pcl.join " | "

         flg = []
         tok = tks.next.expect_after tok, :identi
         for c, i in tok.text.chars.each_with_index
               if c == "-"           then next
            elsif c == "A" && i == 0 then flg.push c
            elsif c == "D" && i == 1 then flg.push c
            elsif c == "U" && i == 2 then flg.push c
            elsif c == "R" && i == 3 then flg.push c
            elsif c == "E" && i == 4 then flg.push c
            else                          tok.raise_kw "funcs" end
         end

         prf = tks.peek_or :number, "0"

         mul = tks.peek_or :period, "0" do |orig|
            tok = tks.next.expect_after orig, :number
            tok.text
         end

         grp = tks.peek_or :assign, "0" do |orig|
            tok = tks.next.expect_after orig, :identi
            "UG_" + tok.text
         end

         req = tks.peek_or :modulo, "0" do |orig|
            req = []
            tok = orig
            tks.while_drop :bar do
               tok = tks.next.expect_after tok, :identi
               req.push "dst_bit(UR_" + tok.text + ")"
            end
            req.join " | "
         end

         upgrades[una] = {inf: inf, scr: scr, pcl: pcl, prf: prf, grp: grp,
                          req: req, mul: mul, flg: flg, cat: category}
      when :eof
         break
      end
   end

   open(ARGV.shift, "wt").puts <<_end_h_; open(ARGV.shift, "wt").puts <<_end_c_; open(ARGV.shift, "wt").puts <<_end_f_
#{generated_header "upgc"}
/* decompat-out pk7/lzscript/Constants/u_names.zsc */

enum /* UpgradeName */ {
#{
res = String.new
upgrades.each_key do |una| res.concat "   UPGR_#{una},\n" end
res.concat "   UPGR_MAX"
res
}
};

/* EOF */
_end_h_
#{generated_header "upgc"}

#include "u_common.h"

/* Extern Objects ---------------------------------------------------------- */

struct upgradeinfo upgrinfo[UPGR_MAX] = {
#{
res = String.new
for una, upg in upgrades
   res.concat %(   {{s"#{una}", #{upg[:inf]}, #{upg[:scr]}}, #{upg[:pcl]}, #{upg[:cat]}, #{upg[:prf]}, #{upg[:grp]}, #{upg[:req]}, #{upg[:mul]}, UPGR_#{una}},\n)
end
res
}
};

i32 Upgr_StrToEnum(cstr s) {
   u32 fst = FourCCPtr(s + 0);
   u32 snd = FourCCPtr(s + 4);
   u32 thd = FourCCPtr(s + 8);
#{generate_strhasher upgrades.keys, "UPGR_"}
   return UPGR_MAX;
}

cstr Upgr_EnumToStr(i32 n) {
   switch(n) {
#{
res = String.new
upgrades.each_key do |una| res.concat "      case UPGR_#{una}: return \"#{zstr una.ljust 12, "\0"}\";\n" end
res
}
   }
   return nil;
}

/* EOF */
_end_c_
#{generated_header "upgc"}

#include "u_func_beg.h"

#{
res = String.new
for una, upg in upgrades
   unless upg[:flg].empty?
      flg = upg[:flg].map do |a| "   #{a}(#{una})\n" end.join
      res.concat "Case(" + una +")\n" + flg
   end
end
res
}

#include "u_func_end.h"

/* EOF */
_end_f_
end

## EOF
