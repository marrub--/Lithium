// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Pickup message handling.                                                 │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"

static
str get_name(i32 w) {
   noinit static
   char name[16];

   faststrcpy(name, weaponinfo[w].typename);
   ifauto(str, alias, lang(strp(_l(LANG "PK_"), name, _l("_ALI")))) {
      faststrcpy_str(name, alias);
   }

   i32 num = faststrtoi32_str(ns(lang(strp(_l(LANG "PK_"), _p((cstr)name), _l("_NUM")))));
   return lang_fmt(LANG "PK_%s_%.3i", name, ACS_Random(0, num));
}

static
void silly_pickup(i32 weapon) {
   i32 fmtnum = faststrtoi32_str(sl_pk_get_num);
   i32 uncnum = faststrtoi32_str(sl_pk_uncertain_num);

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

      /**/ if(flag & 1 && flag & 4) P_LogB(1, fmt, nam, nam, unc);
      else if(flag & 1            ) P_LogB(1, fmt, nam, nam);
      else if(            flag & 4) P_LogB(1, fmt, nam, unc);
      else                          P_LogB(1, fmt, nam);
   }
}

void P_Log_Weapon(i32 which) {
   struct weaponinfo const *info = &weaponinfo[which];
   i32 itemdisp = CVarGetI(sc_player_itemdisp);
   if(itemdisp == _itm_disp_none) {
      return;
   }
   if(CVarGetI(sc_player_sillypickups)) {
      silly_pickup(which);
   } else if(info->name) {
      str nam = ns(lang(fast_strdup2(LANG "INFO_SHORT_", info->name)));
      if(itemdisp & _itm_disp_pop) {
         P_ItemPopup(nam, GetX(0), GetY(0), GetZ(0));
      }
      if(itemdisp & _itm_disp_log) {
         P_LogB(1, tmpstr(sl_pk_get_000), nam);
      }
   } else {
      P_LogB(1, "Acquired impossible object");
   }
}

void P_Log_SellWeapon(i32 which, score_t score) {
   struct weaponinfo const *info = &weaponinfo[which];
   str nam;
   if(CVarGetI(sc_player_sillypickups)) {
      nam = get_name(which);
   } else {
      nam = lang(fast_strdup2(LANG "INFO_SHORT_", info->name));
   }
   nam = ns(nam);
   /* FIXME */
   P_LogB(1, "Sold the %S for %" FMT_SCR "\Cnscr\C-.", nam, score);
}

/* EOF */
