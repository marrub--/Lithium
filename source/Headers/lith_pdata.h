// zsc output: pk7/lzscript/Headers/lith_pdata.h

// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
enum // Lith_PData
{
   pdata_upgrade,
   pdata_riflemode,
   pdata_hassigil,
   pdata_weaponzoom,
   pdata_recoil,
   pdata_pclass,
   pdata_semifrozen,
   pdata_slot3ammo,
};

enum // Lith_PClass
{
   pcl_unknown,

   // Base Classes
   pcl_marine    = 1 << 0,
   pcl_cybermage = 1 << 1,
   pcl_informant = 1 << 2,
   pcl_wanderer  = 1 << 3,
   pcl_assassin  = 1 << 4,
   pcl_darklord  = 1 << 5,
   pcl_thoth     = 1 << 6,

   // Mods
   pcl_fdoomer = 1 << 7,
   pcl_drla    = 1 << 8,

   // Groups
   pcl_outcasts   = pcl_marine    | pcl_cybermage,
   pcl_missioners = pcl_informant | pcl_wanderer,
   pcl_intruders  = pcl_assassin  | pcl_darklord | pcl_thoth,
   pcl_mods       = pcl_fdoomer   | pcl_drla,

   // Lifeform Type
   pcl_human    = pcl_marine   | pcl_cybermage | pcl_assassin | pcl_mods,
   pcl_nonhuman = pcl_wanderer | pcl_darklord  | pcl_thoth,
   pcl_robot    = pcl_informant,

   // Misc. Abilities
   pcl_any       = pcl_outcasts  | pcl_missioners | pcl_intruders | pcl_mods,
   pcl_magicuser = pcl_cybermage | pcl_wanderer   | pcl_thoth,
};

// EOF
