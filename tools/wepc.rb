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

WEPNAMES = %W"wepnam_fist wepnam_chainsaw wepnam_pistol wepnam_shotgun
              wepnam_supershotgun wepnam_chaingun wepnam_rocketlauncher
              wepnam_plasmarifle wepnam_bfg9000"

def out_header fp, weps
   fp.puts <<_end_
/* pk7/lzscript/Constants/p_weapons.zsc
 */
#{generated_header "wepc"}

enum /* WeaponNum */
{
   weapon_min = 1,
   weapon_unknown = 0,

#{
   ret = String.new
   weps.each do |wep| ret << "   #{wep[:nam]},\n" end
   ret
}
   weapon_max
};

enum /* WeaponName */
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

enum /* RifleMode */
{
   rifle_firemode_auto,
   rifle_firemode_grenade,
   rifle_firemode_burst,
   rifle_firemode_max
};

/* EOF */
_end_
end

def out_source fp, weps, wepn
   fp.puts <<_end_
#{generated_header "wepc"}

#if LITHIUM
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
   weps.each do |wep| ret << "   {#{wep[:slt]},#{wep[:pcl]},#{wep[:res].join ","}},\n" end
   ret
}
};

/* Extern Functions -------------------------------------------------------- */

i32 P_Wep_FromName(struct player *p, i32 name) {
   switch(p->pclass) {
#{
   ret = String.new
   for pcl, wpns in wepn
      ret << "   case #{pcl}:\n      switch(name) {\n"
      wpns.each_with_index do |wep, i| ret << "      case #{WEPNAMES[i]}: return #{wep};\n" end
      ret << "      }\n"
   end
   ret
}   }

   return weapon_unknown;
}
#endif

/* EOF */
_end_
end

def new_wep pcl, wf, spl
   spl.push "F(#{wf})" unless wf.empty?
   {pcl: pcl.dup,
    nam: "weapon_#{spl.shift}",
    slt: spl.shift,
    res: [%<N("#{spl.shift}")>, spl.shift, "AT_#{spl.shift}", *spl]}
end

def proc_file ifp, ofh, ofc
   pcl  = "pcl_any"
   wf   = ""
   weps = []
   wepn = {}

   for ln in ifp
      ln.chomp!
      next if ln.empty?

      res = ln[1..-1]
      case ln[0]
      when ":"; pcl = "pcl_#{res}"
      when "%"; wf  = res.dup
      when "+"; weps.push new_wep pcl, wf, res.split
      when ";"; wepn[pcl] = res.split.map do |s| "weapon_#{s}" end
      end
   end

   out_header ofh, weps
   out_source ofc, weps, wepn
end

proc_file open(ARGV[0], "rt"), open(ARGV[1], "wt"), open(ARGV[2], "wt")

## EOF
