enum
{
   weapon_unknown,
   weapon_min    = 1,
   weapon_pistol = weapon_min,
   weapon_shotgun,
// weapon_ssg,
   weapon_rifle,
   weapon_launcher,
   weapon_plasma,
   weapon_bfg,
   weapon_max
};

enum
{
   weaponf_unknown  = 1 << weapon_unknown,
   weaponf_pistol   = 1 << weapon_pistol,
   weaponf_shotgun  = 1 << weapon_shotgun,
// weaponf_ssg      = 1 << weapon_ssg,
   weaponf_rifle    = 1 << weapon_rifle,
   weaponf_launcher = 1 << weapon_launcher,
   weaponf_plasma   = 1 << weapon_plasma,
   weaponf_bfg      = 1 << weapon_bfg
};

enum
{
   rifle_firemode_auto,
   rifle_firemode_grenade,
   rifle_firemode_burst,
   rifle_firemode_max
};

// EOF

