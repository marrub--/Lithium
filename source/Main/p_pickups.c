// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"

#define name(n) "LITH_PK_" #n

static __str const pickupnames[weapon_max] = {
   [weapon_unknown]    = name(weapon_unknown),

   [weapon_fist]       = name(weapon_unknown),
   [weapon_cfist]      = name(weapon_cfist),
   [weapon_pistol]     = name(weapon_pistol),
   [weapon_revolver]   = name(weapon_revolver),
   [weapon_shotgun]    = name(weapon_shotgun),
   [weapon_ssg]        = name(weapon_ssg),
   [weapon_rifle]      = name(weapon_rifle),
   [weapon_launcher]   = name(weapon_launcher),
   [weapon_plasma]     = name(weapon_plasma),
   [weapon_bfg]        = name(weapon_bfg),

   [weapon_c_fist]     = name(weapon_unknown),
   [weapon_c_mateba]   = name(weapon_c_mateba),
   [weapon_c_rifle]    = name(weapon_c_rifle),
   [weapon_c_spas]     = name(weapon_shotgun),
   [weapon_c_smg]      = name(weapon_c_smg),
   [weapon_c_sniper]   = name(weapon_c_sniper),
   [weapon_c_plasma]   = name(weapon_plasma),
   [weapon_c_shipgun]  = name(weapon_c_shipgun),
};

static void Lith_StupidPickup(struct player *p, int weapon)
{
   int num = strtoi_str(Language("%S_NUM", pickupnames[weapon]), null, 10);

   static int fmtnum;
   if(!fmtnum) fmtnum = strtoi_str(L("LITH_PK_GET_NUM"), null, 10);

   static int uncnum;
   if(!uncnum) uncnum = strtoi_str(L("LITH_PK_UNCERTAIN_NUM"), null, 10);

   int iname = ACS_Random(0, num);
   int iunc  = ACS_Random(0, uncnum);
   int ifmt  = ACS_Random(0, fmtnum);
   int flag  = strtoi_str(Language("LITH_PK_GET_%.3i_FLAGS", ifmt), null, 0);

   if(flag & 2) {
      ifmt = ACS_Random(0, fmtnum);
      flag = strtoi_str(Language("LITH_PK_GET_%.3i_FLAGS", ifmt), null, 0);
   }

   __str fmt = Language("LITH_PK_GET_%.3i", ifmt);
   __str nam = Language("%S_%.3i", pickupnames[weapon], iname);
   __str unc = Language("LITH_PK_UNCERTAIN_%.3i", iunc);

        if(flag & 1 && flag & 4) p->log(1, fmt, nam, nam, unc);
   else if(flag & 1            ) p->log(1, fmt, nam, nam);
   else if(            flag & 4) p->log(1, fmt, nam, unc);
   else                          p->log(1, fmt, nam);
}

void Lith_PickupMessage(struct player *p, weaponinfo_t const *info)
{
   if(p->getCVarI("lith_player_stupidpickups"))
      Lith_StupidPickup(p, info->type);
   else if(info->name)
      p->log(1, StrParam("%S", L("LITH_PK_GET_000")),
         Language("LITH_INFO_SHORT_%S", info->name));
   else
      p->log(1, "Acquired impossible object");
}

// EOF

