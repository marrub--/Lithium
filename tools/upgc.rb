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
## Upgrade info text compiler.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

def parse_upgrade tks, tok
   tok = tks.next.expect_after tok, :identi
   nam = tok.text

   tok = tks.next.expect_after tok, :identi
   inf = if tok.text == "N/A" then "0" else '"' + tok.text + '"' end

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
         req.push "UR_" + tok.text
      end
      req.join " | "
   end

   {nam: nam, inf: inf, scr: scr, pcl: pcl, prf: prf, grp: grp, req: req,
    mul: mul, flg: flg}
end

common_main do
   tks = tokenize ARGV.shift

   upgrades = []
   category = ""

   loop do
      tok = tks.next.expect_in_top [:colon, :plus, :eof]

      case tok.type
      when :colon
         tok = tks.next.expect_after tok, :identi
         category = "UC_" + tok.text
      when :plus
         upg = parse_upgrade tks, tok
         upg[:cat] = category
         upgrades.push upg
      when :eof
         break
      end
   end

   open(ARGV.shift, "wt").puts <<_end_h_; open(ARGV.shift, "wt").puts <<_end_c_; open(ARGV.shift, "wt").puts <<_end_f_
/* pk7/lzscript/Constants/u_names.zsc
 */
#{generated_header "upgc"}

enum /* Lith_UpgradeName */
{
#{
res = String.new
for upg in upgrades
   res.concat "   UPGR_#{upg[:nam]},\n"
end
res.concat "   UPGR_BASE_MAX"
res
}
};

/* EOF */
_end_h_
#{generated_header "upgc"}

#include "u_common.h"

StrEntON

/* Extern Objects ---------------------------------------------------------- */

struct upgradeinfo const upgrinfobase[UPGR_BASE_MAX] = {
#{
res = String.new
for upg in upgrades
   res.concat %(   {{"#{upg[:nam]}", #{upg[:inf]}, #{upg[:scr]}}, #{upg[:pcl]}, #{upg[:cat]}, #{upg[:prf]}, #{upg[:grp]}, #{upg[:req]}, #{upg[:mul]}, UPGR_#{upg[:nam]}},\n)
end
res
}
};

/* EOF */
_end_c_
#{generated_header "upgc"}

#include "u_func_beg.h"

#{
res = String.new
for upg in upgrades
   unless upg[:flg].empty?
      nam = upg[:nam]
      flg = upg[:flg].map do |a| "   #{a}(#{nam})" end.join "\n"
      res.concat "Case(" + nam +")\n" + flg
   end
end
res
}

#include "u_func_end.h"

/* EOF */
_end_f_
end

## EOF
