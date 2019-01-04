// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#if LITHIUM
#include "common.h"
#include "p_player.h"

#define name(n) sLANG "PK_" #n

#define StupidName(w) \
   Language("%S_%.3i", pickupnames[w], \
      ACS_Random(0, strtoi_str(Language(c"%S_NUM", pickupnames[w]), nil, 10)))

static str const pickupnames[weapon_max] = {
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

static void Lith_StupidPickup(struct player *p, i32 weapon)
{
   i32 fmtnum = strtoi(LC(LANG "PK_GET_NUM"),       nil, 10);
   i32 uncnum = strtoi(LC(LANG "PK_UNCERTAIN_NUM"), nil, 10);

   i32 iunc  = ACS_Random(0, uncnum);
   i32 ifmt  = ACS_Random(0, fmtnum);
   i32 flag  = strtoi(LanguageC(LANG "PK_GET_%.3i_FLAGS", ifmt), nil, 0);

   if(flag & 2) {
      ifmt = ACS_Random(0, fmtnum);
      flag = strtoi(LanguageC(LANG "PK_GET_%.3i_FLAGS", ifmt), nil, 0);
   }

   str nam = StupidName(weapon);

   char const *fmt = LanguageC(LANG "PK_GET_%.3i", ifmt);
   str         unc = Language (LANG "PK_UNCERTAIN_%.3i", iunc);

        if(flag & 1 && flag & 4) p->logB(1, fmt, nam, nam, unc);
   else if(flag & 1            ) p->logB(1, fmt, nam, nam);
   else if(            flag & 4) p->logB(1, fmt, nam, unc);
   else                          p->logB(1, fmt, nam);
}

void Lith_PickupMessage(struct player *p, struct weaponinfo const *info)
{
   if(p->getCVarI(sc_player_stupidpickups))
      Lith_StupidPickup(p, info->type);
   else if(info->name)
      p->logB(1, LC(LANG "PK_GET_000"), Language(LANG "INFO_SHORT_%S", info->name));
   else
      p->logB(1, "Acquired impossible object");
}

void Lith_SellMessage(struct player *p, struct weaponinfo const *info, i96 score)
{
   i32 weapon = info->type;
   bool ord = strtoi(LC(LANG "LOG_SellOrder"), nil, 10) == 0;

   str nam;

   if(p->getCVarI(sc_player_stupidpickups))
      nam = StupidName(weapon);
   else
      nam = Language(LANG "INFO_SHORT_%S", info->name);

   char const *msg = LC(LANG "LOG_Sell");

   if(ord) p->logB(1, msg, nam, score);
   else    p->logB(1, msg, score, nam);
}
#endif

// EOF
