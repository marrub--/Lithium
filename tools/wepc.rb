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
## Weapon info text compiler.
##
## ---------------------------------------------------------------------------|

require_relative "corinth.rb"
require "yaml"

common_main do
   hsh     = YAML.load(IO.read(ARGV.shift), symbolize_names: true)
   weapons = []
   pickups = {}

   for pclass, weapondata in hsh
      pcl = pclass.to_s
      for name, hwep in weapondata
         if name == :pickups
            pickups[pclass] = hwep
         else
            name = name.to_s
            ammo = hwep[2].split
            ammo[0].prepend "AT_"
            if ammo[1]
               ammo[1] = 'sOBJ "' + ammo[1] + 'Ammo"'
            else
               ammo.push "snil"
            end
            cname = '"' + hwep[1] + '"'
            wep = {}
            wep[:slt] = hwep[0]
            wep[:pcl] = pcl
            wep[:nam] = cname
            wep[:snd] = 's"weapons/' + name[2..] + '/pickup"'
            wep[:aty] = ammo[0]
            wep[:acl] = ammo[1]
            wep[:cls] = 'sOBJ ' + cname
            wep[:typ] = name
            weapons.push wep
         end
      end
   end

   open(ARGV.shift, "wt").puts <<_end_h_; open(ARGV.shift, "wt").puts <<_end_c_
#{generated_header "wepc"}

#ifndef wepc_header
#define wepc_header

enum ZscName(WeaponNum) {
#{
res = String.new
for wep in weapons do res.concat "   weapon_#{wep[:typ]},\n" end
res
}
   weapon_max,
   weapon_min = 1,
};

#endif
_end_h_
#{generated_header "wepc"}

#include "common.h"
#include "p_player.h"

/* Extern Objects ---------------------------------------------------------- */

struct weaponinfo const weaponinfo[weapon_max] = {
#{
res = String.new
for wep in weapons do res.concat "   {#{wep[:slt]}, #{wep[:pcl]}, #{wep[:nam]}, #{wep[:snd]}, #{wep[:aty]}, #{wep[:acl]}, #{wep[:cls]}, weapon_#{wep[:typ]}, \"#{wep[:typ]}\"},\n" end
res
}
};

/* Extern Functions -------------------------------------------------------- */

i32 Wep_FromName(i32 name) {
   switch(pl.pclass) {
#{
res = String.new
for pcl, map in pickups
   res.concat "   case #{pcl}:\n"
   res.concat "      switch(name) {\n"
   for name, wep in map
      res.concat "      case wepnam_#{name}: return weapon_#{wep};\n"
   end
   res.concat "      }\n"
end
res
}
   }
   return weapon_unknown;
}

/* EOF */
_end_c_
end

## EOF
