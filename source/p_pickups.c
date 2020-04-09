/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Pickup message handling.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"

static str get_name(i32 w) {
   char name[20];
   strcpy(name, P_Wep_GetPickup(w));
   ifauto(str, alias, LanguageNull(LANG "PK_%s_ALI", name))
      lstrcpy_str(name, alias);

   i32 num = strtoi(LanguageC(LANG "PK_%s_NUM", name), nil, 10);
   return Language(LANG "PK_%s_%.3i", name, ACS_Random(0, num));
}

static void silly_pickup(struct player *p, i32 weapon)
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

   str nam = get_name(weapon);

   cstr fmt = LanguageC(LANG "PK_GET_%.3i", ifmt);
   str  unc = Language (LANG "PK_UNCERTAIN_%.3i", iunc);

        if(flag & 1 && flag & 4) p->logB(1, fmt, nam, nam, unc);
   else if(flag & 1            ) p->logB(1, fmt, nam, nam);
   else if(            flag & 4) p->logB(1, fmt, nam, unc);
   else                          p->logB(1, fmt, nam);
}

void P_Log_Weapon(struct player *p, struct weaponinfo const *info)
{
   if(p->getCVarI(sc_player_stupidpickups))
      silly_pickup(p, info->type);
   else if(info->name)
      p->logB(1, LC(LANG "PK_GET_000"), Language(LANG "INFO_SHORT_%S", info->name));
   else
      p->logB(1, "Acquired impossible object");
}

void P_Log_SellWeapon(struct player *p, struct weaponinfo const *info, i96 score)
{
   i32 weapon = info->type;
   bool ord = strtoi(LC(LANG "LOG_SellOrder"), nil, 10) == 0;

   str nam;

   if(p->getCVarI(sc_player_stupidpickups))
      nam = get_name(weapon);
   else
      nam = Language(LANG "INFO_SHORT_%S", info->name);

   cstr msg = LC(LANG "LOG_Sell");

   if(ord) p->logB(1, msg, nam, score);
   else    p->logB(1, msg, score, nam);
}

/* EOF */
