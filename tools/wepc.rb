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
## Weapon info text compiler.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"

common_main do
   WEPNAMES = %W"wepnam_fist wepnam_chainsaw wepnam_pistol wepnam_shotgun
               wepnam_supershotgun wepnam_chaingun wepnam_rocketlauncher
               wepnam_plasmarifle wepnam_bfg9000"

   tks = tokenize ARGV.shift

   pcl  = "pcl_any"
   wf   = []
   weps = []
   wepn = {}

   loop do
      tok = tks.next.expect_in_top [:colon, :modulo, :plus, :semico, :eof]

      case tok.type
      when :colon
         tok = tks.next.expect_after tok, :identi
         pcl = "pcl_" + tok.text
      when :modulo
         wf = []
         if tks.peek.type == :identi
            tks.while_drop :bar do
               tok = tks.next.expect_after tok, :identi
               wf.push tok.text
            end
         end
      when :plus
         res = []
         tok = tks.next.expect_after tok, :identi
         nam = "weapon_" + tok.text
         tok = tks.next.expect_after tok, :number
         slt = tok.text
         tok = tks.next.expect_after tok, :identi
         res.push 'N("' + tok.text + '")'
         tok = tks.next.expect_after tok, [:identi, :string]
         if tok.type == :identi
            res.push tok.text
         else
            res.push 'P("' + tok.text + '")'
         end
         tok = tks.next.expect_after tok, :identi
         res.push "AT_" + tok.text
         tks.peek_or :identi do |orig|
            typ = orig.text
            tok = tks.next.expect_after orig, :string
            res.push typ + '("' + tok.text + '")'
         end
         res.push "F(" + wf.join(" | ") + ")" unless wf.empty?
         weps.push({pcl: pcl, nam: nam, slt: slt, res: res})
      when :semico
         res = []
         while tks.peek.type == :identi
            tok = tks.next
            res.push "weapon_" + tok.text
         end
         wepn[pcl] = res
      when :eof
         break
      end
   end

   open(ARGV.shift, "wt").puts <<_end_h_; open(ARGV.shift, "wt").puts <<_end_c_
/* pk7/lzscript/Constants/p_weapons.zsc
 */
#{generated_header "wepc"}

enum /* Lith_WeaponNum */
{
   weapon_min = 1,
   weapon_unknown = 0,

#{
   ret = String.new
   weps.each do |wep| ret.concat "   #{wep[:nam]},\n" end
   ret
}
   weapon_max
};

enum /* Lith_WeaponName */
{
   wepnam_fist,
   wepnam_chainsaw,
   wepnam_pistol,
   wepnam_shotgun,
   wepnam_supershotgun,
   wepnam_chaingun,
   wepnam_rocketlauncher,
   wepnam_plasmarifle,
   wepnam_bfg9000,

   wepnam_max,
};

enum /* Lith_RifleMode */
{
   rifle_firemode_auto,
   rifle_firemode_grenade,
   rifle_firemode_burst,
   rifle_firemode_max
};

/* EOF */
_end_h_
#{generated_header "wepc"}

#include "common.h"
#include "p_player.h"

StrEntON

/* Extern Objects ---------------------------------------------------------- */

#define Placeholder1 "MMMMHMHMMMHMMM"
#define Placeholder2 "YOUSONOFABITCH"
#define A(a) OBJ a "Ammo"
#define O(a) OBJ a
#define P(a) "weapons/" a "/pickup"
#define N(a) .classname = OBJ a, .name = a
#define F(...) .flags = __VA_ARGS__
struct weaponinfo const weaponinfo[weapon_max] = {
   {0, pcl_any, snil, "MMMMHMHMMMHMMM"},
#{
   ret = String.new
   weps.each do |wep|
      ret.concat "   {#{wep[:slt]}, #{wep[:pcl]}, #{wep[:res].join ","}},\n"
   end
   ret
}
};

/* Extern Functions -------------------------------------------------------- */

i32 P_Wep_FromName(struct player *p, i32 name) {
   switch(p->pclass) {
#{
   ret = String.new
   for pcl, wpns in wepn
      ret.concat "   case #{pcl}:\n      switch(name) {\n"
      wpns.each_with_index do |wep, i|
         ret.concat "      case #{WEPNAMES[i]}: return #{wep};\n"
      end
      ret.concat "      }\n"
   end
   ret
}   }

   return weapon_unknown;
}

/* EOF */
_end_c_
end

## EOF
