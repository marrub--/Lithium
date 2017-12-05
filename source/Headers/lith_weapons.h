// dec output: pk7/lscripts/Headers/lith_weapons.h
// zsc output: pk7/lzscript/Headers/lith_weapons.h

// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
enum // Lith_WeaponNum
{
   weapon_unknown,
   weapon_min,

   // Outcast Weapons
   weapon_cfist = weapon_min,
   weapon_missile,
   weapon_plasmadiff,

   // Marine Weapons
   weapon_fist,
   weapon_pistol,
   weapon_revolver,
   weapon_shotgun,
   weapon_lazshotgun,
   weapon_ssg,
   weapon_rifle,
   weapon_sniper,
   weapon_launcher,
   weapon_plasma,
   weapon_bfg,

   // Cyber-Mage Weapons
   weapon_c_fist,
   weapon_c_mateba,
   weapon_c_rifle,
   weapon_c_spas,
   weapon_c_smg,
   weapon_c_sniper,
   weapon_c_plasma,
   weapon_c_shipgun,

   weapon_c_blade,
   weapon_c_delear,
   weapon_c_fire,
   weapon_c_rend,
   weapon_c_hulgyon,
   weapon_c_starshot,
   weapon_c_cercle,

   // Dark Lord Weapons
   weapon_d_700e,

   // Misc. Weapons
   weapon_wings,

   weapon_max
};

enum // Lith_WeaponName
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

enum // Lith_RifleMode
{
   rifle_firemode_auto,
   rifle_firemode_grenade,
   rifle_firemode_burst,
   rifle_firemode_max
};

// EOF
