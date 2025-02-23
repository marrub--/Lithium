#!/usr/bin/env -S ruby --enable=frozen-string-literal -w
## ╭──────────────────────────────────────────────────────────────────────────╮
## │                                                                          │
## │             Distributed under the CC0 public domain license.             │
## │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
## │                See licenses/cc0.txt for more information.                │
## │                                                                          │
## ├──────────────────────────────────────────────────────────────────────────┤
## │                                                                          │
## │ Weapon info text compiler.                                               │
## │                                                                          │
## ╰──────────────────────────────────────────────────────────────────────────╯

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
            if ammo[0] != 'None'
               ammo[0] = ammo[0]
                  .split('|', -1)
                  .map do |s| s.strip.prepend 'AT_' end
                  .join('|')
            else
               ammo[0] = 0
            end
            if ammo[1]
               ammo[1] = 's"" OBJ "' + ammo[1] + 'Ammo"'
            else
               ammo.push "snil"
            end
            wep = {}
            wep[:slt] = hwep[0]
            wep[:pcl] = pcl
            wep[:nam] = '"' + hwep[1] + '"'
            wep[:snd] = 's"weapons/' + name[2..] + '/pickup"'
            wep[:aty] = ammo[0]
            wep[:acl] = ammo[1]
            wep[:cls] = 's"LITH_' + hwep[1].upcase + '"'
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

#include "m_engine.h"
#include "p_player.h"

struct weaponinfo const weaponinfo[weapon_max] = {
#{
res = String.new
for wep in weapons do res.concat "   {#{wep[:slt]}, #{wep[:pcl]}, #{wep[:nam]}, #{wep[:snd]}, #{wep[:aty]}, #{wep[:acl]}, #{wep[:cls]}, \"#{wep[:typ]}\"},\n" end
res
}
};

i32 Wep_FromName(i32 name) {
   switch(dst_bit(pl.pclass)) {
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
