enum
{
   pdata_upgrade,
   pdata_rifle_firemode,
   pdata_buttons,
   pdata_has_sigil,
   pdata_weapon_zoom,
   pdata_pclass
};

enum
{
   pcl_unknown,
   pcl_marine    = 1 << 0,
   pcl_cybermage = 1 << 1,
   pcl_informant = 1 << 2,
   pcl_wanderer  = 1 << 3,
   pcl_assassin  = 1 << 4,
   pcl_darklord  = 1 << 5,
   pcl_thoth     = 1 << 6,
   pcl_outcasts   = pcl_marine | pcl_cybermage,
   pcl_missioners = pcl_informant | pcl_wanderer,
   pcl_intruders  = pcl_assassin | pcl_darklord | pcl_thoth,
   pcl_any        = pcl_outcasts | pcl_missioners | pcl_intruders,
};

