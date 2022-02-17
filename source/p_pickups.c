// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Pickup message handling.                                                 │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"

static
str get_name(i32 w) {
   noinit static
   char name[16];

   faststrcpy(name, weaponinfo[w].typename);
   ifauto(str, alias, lang_fmt(LANG "PK_%s_ALI", name)) {
      faststrcpy_str(name, alias);
   }

   i32 num = faststrtoi32_str(ns(lang_fmt(LANG "PK_%s_NUM", name)));
   return lang_fmt(LANG "PK_%s_%.3i", name, ACS_Random(0, num));
}

static
void silly_pickup(i32 weapon) {
   i32 fmtnum = faststrtoi32_str(ns(lang(sl_pk_get_num)));
   i32 uncnum = faststrtoi32_str(ns(lang(sl_pk_uncertain_num)));

   i32 iunc  = ACS_Random(0, uncnum);
   i32 ifmt  = ACS_Random(0, fmtnum);
   i32 flag  = faststrtoi32_str(ns(lang_fmt(LANG "PK_GET_%.3i_FLAGS", ifmt)));

   if(flag & 2) {
      ifmt = ACS_Random(0, fmtnum);
      flag = faststrtoi32_str(ns(lang_fmt(LANG "PK_GET_%.3i_FLAGS", ifmt)));
   }

   str nam = ns(get_name(weapon));

   i32 itemdisp = CVarGetI(sc_player_itemdisp);
   if(itemdisp & _itm_disp_pop) {
      P_ItemPopup(nam, GetX(0), GetY(0), GetZ(0));
   }
   if(itemdisp & _itm_disp_log) {
      cstr fmt = tmpstr(lang_fmt(LANG "PK_GET_%.3i", ifmt));
      str  unc =     ns(lang_fmt(LANG "PK_UNCERTAIN_%.3i", iunc));

      if(flag & 1 && flag & 4) pl.logB(1, fmt, nam, nam, unc);
      else if(flag & 1            ) pl.logB(1, fmt, nam, nam);
      else if(            flag & 4) pl.logB(1, fmt, nam, unc);
      else                          pl.logB(1, fmt, nam);
   }
}

void P_Log_Weapon(struct weaponinfo const *info) {
   i32 itemdisp = CVarGetI(sc_player_itemdisp);
   if(itemdisp == _itm_disp_none) {
      return;
   }

   if(CVarGetI(sc_player_sillypickups)) {
      silly_pickup(info->type);
   } else if(info->name) {
      str nam = ns(lang_fmt(LANG "INFO_SHORT_%s", info->name));
      if(itemdisp & _itm_disp_pop) {
         P_ItemPopup(nam, GetX(0), GetY(0), GetZ(0));
      }
      if(itemdisp & _itm_disp_log) {
         pl.logB(1, tmpstr(lang(sl_pk_get_000)), nam);
      }
   } else {
      pl.logB(1, "Acquired impossible object");
   }
}

void P_Log_SellWeapon(struct weaponinfo const *info, i96 score) {
   i32 weapon = info->type;
   bool ord = faststrtoi32_str(ns(lang(sl_log_sellorder))) == 0;

   str nam;

   if(CVarGetI(sc_player_sillypickups))
      nam = get_name(weapon);
   else
      nam = lang_fmt(LANG "INFO_SHORT_%s", info->name);
   nam = ns(nam);

   cstr msg = tmpstr(lang(sl_log_sell));

   if(ord) pl.logB(1, msg, nam, score);
   else    pl.logB(1, msg, score, nam);
}

/* EOF */
