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

static
str get_name(i32 w) {
   char name[20];
   faststrcpy(name, P_Wep_GetPickup(w));
   ifauto(str, alias, LanguageNull(LANG "PK_%s_ALI", name))
      faststrcpy_str(name, alias);

   i32 num = faststrtoi32(LanguageC(LANG "PK_%s_NUM", name));
   return Language(LANG "PK_%s_%.3i", name, ACS_Random(0, num));
}

static
void silly_pickup(i32 weapon) {
   i32 fmtnum = faststrtoi32(LC(LANG "PK_GET_NUM"));
   i32 uncnum = faststrtoi32(LC(LANG "PK_UNCERTAIN_NUM"));

   i32 iunc  = ACS_Random(0, uncnum);
   i32 ifmt  = ACS_Random(0, fmtnum);
   i32 flag  = faststrtoi32(LanguageC(LANG "PK_GET_%.3i_FLAGS", ifmt));

   if(flag & 2) {
      ifmt = ACS_Random(0, fmtnum);
      flag = faststrtoi32(LanguageC(LANG "PK_GET_%.3i_FLAGS", ifmt));
   }

   str nam = get_name(weapon);

   cstr fmt = LanguageC(LANG "PK_GET_%.3i", ifmt);
   str  unc = Language (LANG "PK_UNCERTAIN_%.3i", iunc);

        if(flag & 1 && flag & 4) pl.logB(1, fmt, nam, nam, unc);
   else if(flag & 1            ) pl.logB(1, fmt, nam, nam);
   else if(            flag & 4) pl.logB(1, fmt, nam, unc);
   else                          pl.logB(1, fmt, nam);
}

void P_Log_Weapon(struct weaponinfo const *info) {
   if(CVarGetI(sc_player_stupidpickups))
      silly_pickup(info->type);
   else if(info->name)
      pl.logB(1, LC(LANG "PK_GET_000"), Language(LANG "INFO_SHORT_%S", info->name));
   else
      pl.logB(1, "Acquired impossible object");
}

void P_Log_SellWeapon(struct weaponinfo const *info, i96 score) {
   i32 weapon = info->type;
   bool ord = faststrtoi32(LC(LANG "LOG_SellOrder")) == 0;

   str nam;

   if(CVarGetI(sc_player_stupidpickups))
      nam = get_name(weapon);
   else
      nam = Language(LANG "INFO_SHORT_%S", info->name);

   cstr msg = LC(LANG "LOG_Sell");

   if(ord) pl.logB(1, msg, nam, score);
   else    pl.logB(1, msg, score, nam);
}

/* EOF */
