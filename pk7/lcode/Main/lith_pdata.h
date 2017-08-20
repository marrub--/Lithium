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
   pclass_unknown,
   pclass_marine    = 1 << 0,
   pclass_cybermage = 1 << 1,
   pclass_informant = 1 << 2,
   pclass_wanderer  = 1 << 3,
   pclass_assassin  = 1 << 4,
   pclass_darklord  = 1 << 5,
   pclass_thoth     = 1 << 6,
   pclass_any = pclass_marine    |
                pclass_cybermage |
                pclass_informant |
                pclass_wanderer  |
                pclass_assassin  |
                pclass_darklord  |
                pclass_thoth
};

