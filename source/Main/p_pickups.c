// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "lith_common.h"
#include "lith_player.h"

StrEntON

#define name(n) LANG "PK_" #n

#define StupidName(w) \
   Language("%S_%.3i", pickupnames[w], \
      ACS_Random(0, strtoi_str(Language("%S_NUM", pickupnames[w]), null, 10)))

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
   int fmtnum = strtoi_str(L(LANG "PK_GET_NUM"),       null, 10);
   int uncnum = strtoi_str(L(LANG "PK_UNCERTAIN_NUM"), null, 10);

   int iunc  = ACS_Random(0, uncnum);
   int ifmt  = ACS_Random(0, fmtnum);
   int flag  = strtoi_str(Language(LANG "PK_GET_%.3i_FLAGS", ifmt), null, 0);

   if(flag & 2) {
      ifmt = ACS_Random(0, fmtnum);
      flag = strtoi_str(Language(LANG "PK_GET_%.3i_FLAGS", ifmt), null, 0);
   }

   __str nam = StupidName(weapon);
   __str fmt = Language(LANG "PK_GET_%.3i", ifmt);
   __str unc = Language(LANG "PK_UNCERTAIN_%.3i", iunc);

        if(flag & 1 && flag & 4) p->logB(1, fmt, nam, nam, unc);
   else if(flag & 1            ) p->logB(1, fmt, nam, nam);
   else if(            flag & 4) p->logB(1, fmt, nam, unc);
   else                          p->logB(1, fmt, nam);
}

void Lith_PickupMessage(struct player *p, weaponinfo_t const *info)
{
   if(p->getCVarI(sCVAR "player_stupidpickups"))
      Lith_StupidPickup(p, info->type);
   else if(info->name)
      p->logB(1, L(LANG "PK_GET_000"), Language(LANG "INFO_SHORT_%S", info->name));
   else
      p->logB(1, "Acquired impossible object");
}

void Lith_SellMessage(struct player *p, weaponinfo_t const *info, i96 score)
{
   int weapon = info->type;
   bool ord = strtoi_str(L(LANG "LOG_SellOrder"), null, 10) == 0;

   __str nam;

   if(p->getCVarI(sCVAR "player_stupidpickups"))
      nam = StupidName(weapon);
   else
      nam = Language(LANG "INFO_SHORT_%S", info->name);

   __str msg = L(LANG "LOG_Sell");

   if(ord) p->logB(1, msg, nam, score);
   else    p->logB(1, msg, score, nam);
}
#endif

// EOF
