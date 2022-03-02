// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Settings configuration UI.                                               │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "w_world.h"

enum {
   _left = 2,
   _rght = 280,
};

enum {
   lxh_max = 10,
   lxb_max = 13,
};

script funcdef void (*  scb_t)(struct set_parm const *sp);
script funcdef bool (*scb_b_t)(struct set_parm const *sp, bool *v);
script funcdef i32  (*scb_i_t)(struct set_parm const *sp, i32  *v);
script funcdef k32  (*scb_k_t)(struct set_parm const *sp, k32  *v);
script funcdef str  (*scb_s_t)(struct set_parm const *sp, str  *v);

/* all members are ordered by how many types use them */
struct setting {
   /* used by all */
   scb_t cb;
   /* used by any with text */
   cstr text;
   /* used by any with a variable */
   union {
      scb_b_t b;
      scb_i_t i;
      scb_k_t k;
      scb_s_t s;
   } cb_g;
   /* used by any with a scalar */
   union {
      struct {i32 min, max;} i;
      struct {k32 min, max;} k;
   } bnd;
   cstr suff;
   /* used in special cases */
   bool fill;
   i32  pclass;
};

struct set_parm {
   struct gui_state *g;
   i32 x, y;
   struct setting const *st;
};

#define SG_cvBody(type, name, suff) \
   script static \
   type SG_cv##name(struct set_parm const *sp, type *v) { \
      str cvar = fast_strdup2(CVAR, sp->st->text); \
      if(v) {CVarSet##suff(cvar, *v); return *v;} \
      else  {return CVarGet##suff(cvar);} \
   }

SG_cvBody(bool, Boole, I)
SG_cvBody(i32,  Integ, I)
SG_cvBody(k32,  Fixed, K)
SG_cvBody(str,  Strng, S)

script static
bool SG_autoBuy(struct set_parm const *sp, bool *v) {
   i32 which = sp->st->text[faststrlen(sp->st->text) - 1] - '1';
   if(v) {tog_bit(pl.autobuy, which); P_Data_Save();}
   return get_bit(pl.autobuy, which);
}

script static
bool SG_doneIntro(struct set_parm const *sp, bool *v) {
   if(v) {pl.done_intro ^= pl.pclass; P_Data_Save();}
   return pl.done_intro &  pl.pclass;
}

script static
void S_empty(struct set_parm const *sp) {
}

script static
void S_label(struct set_parm const *sp) {
   PrintText_str(ns(lang_fmt(LANG "%s", sp->st->text)),
                 sf_smallfnt, sp->g->defcr, sp->g->ox + _left,1,
                 sp->g->oy + sp->y,1);
}

script static
void S_boole(struct set_parm const *sp) {
   bool v = sp->st->cb_g.b(sp, nil);

   S_label(sp);

   if(G_Button_HId(sp->g, sp->y, tmpstr(v ?
                                        lang(sl_on) :
                                        lang(sl_off)),
                   _rght - gui_p.btnlist.w, sp->y, Pre(btnlist),
                   .fill = sp->st->fill ? &CBIState(sp->g)->settingsfill : nil))
   {
      v = !v;
      sp->st->cb_g.b(sp, &v);
   }
}

script static
void S_integ(struct set_parm const *sp) {
   i32 v = sp->st->cb_g.i(sp, nil);

   S_label(sp);

   struct slide_ret sret =
      G_Slider_HId(sp->g, sp->y, _rght - gui_p.slddef.w, sp->y,
                   sp->st->bnd.i.min, sp->st->bnd.i.max, v, true,
                   .suf = tmpstr(lang_fmt(LANG "st_suff_%s", sp->st->suff)));
   if(sret.different) {
      v = sret.value;
      sp->st->cb_g.i(sp, &v);
   }
}

script static
void S_fixed(struct set_parm const *sp) {
   k32 v = sp->st->cb_g.k(sp, nil);

   S_label(sp);

   struct slide_ret sret =
      G_Slider_HId(sp->g, sp->y, _rght - gui_p.slddef.w, sp->y,
                   sp->st->bnd.k.min, sp->st->bnd.k.max, v,
                   .suf = tmpstr(lang_fmt(LANG "st_suff_%s", sp->st->suff)));
   if(sret.different) {
      v = sret.value;
      sp->st->cb_g.k(sp, &v);
   }
}

script static
void S_enume(struct set_parm const *sp) {
   i32 v = sp->st->cb_g.i(sp, nil);
   i32 min = sp->st->bnd.i.min;
   i32 max = sp->st->bnd.i.max - 1;
   i32 btw = gui_p.btnnexts.w;

   S_label(sp);

   if(G_Button_HId(sp->g, sp->y, .x = _rght - btw * 2, sp->y, v <= min,
                   Pre(btnprevs))) {
      v--;
      sp->st->cb_g.i(sp, &v);
   }

   if(G_Button_HId(sp->g, sp->y, .x = _rght - btw, sp->y, v >= max,
                   Pre(btnnexts))) {
      v++;
      sp->st->cb_g.i(sp, &v);
   }

   i32 cr = faststrchk(sp->st->suff, "color") ? Draw_GetCr(v) : sp->g->defcr;

   PrintText_str(ns(v < min || v > max ?
                    lang(sl_st_name_unknown) :
                    lang_fmt(LANG "st_name_%s_%i", sp->st->suff, v)),
                 sf_smallfnt, cr,
                 sp->g->ox + _rght - btw * 2 - 1,2,
                 sp->g->oy + sp->y,1);
}

script static
void S_strng(struct set_parm const *sp) {
   str        v   = sp->st->cb_g.s(sp, nil);
   mem_size_t len = ACS_StrLen(v);

   S_label(sp);

   struct gui_txt st;
   st.tbptr = len;
   for(mem_size_t i = 0; i < len; i++) Cps_SetC(st.txtbuf, i, v[i]);

   if(G_TxtBox_HId(sp->g, sp->y, &st, _rght - 132, sp->y)) {
      v = Cps_Expand_str(st.txtbuf, 0, st.tbptr);
      sp->st->cb_g.s(sp, &v);
   }
}

static
bool S_isEnabled(struct setting const *st) {
   return !st->pclass || pl.pclass & st->pclass;
}

#define S_bndi(min, max) .bnd = {.i = {min, max}}
#define S_bndk(min, max) .bnd = {.k = {min, max}}

#define S_color S_bndi(_gcr_first, _gcr_max), "color"

#define S_cvBoole .cb_g = {.b = SG_cvBoole}
#define S_cvInteg .cb_g = {.i = SG_cvInteg}
#define S_cvFixed .cb_g = {.k = SG_cvFixed}
#define S_cvStrng .cb_g = {.s = SG_cvStrng}

struct setting const st_gui[] = {
   {S_label, "st_labl_gui_cbi"},
   {S_enume, "gui_cursor",    S_cvInteg, S_bndi(0, gui_curs_max), "cursor"},
   {S_enume, "gui_defcr",     S_cvInteg, S_color},
   {S_enume, "gui_theme",     S_cvInteg, S_bndi(0, cbi_theme_max), "theme"},
   {S_fixed, "gui_xmul",      S_cvFixed, S_bndk(0.1, 2.0), "mult"},
   {S_fixed, "gui_ymul",      S_cvFixed, S_bndk(0.1, 2.0), "mult"},
   {S_integ, "gui_buyfiller", S_cvInteg, S_bndi(0, 70), "tick"},
   {S_empty},
   {S_label, "st_labl_gui_results"},
   {S_boole, "player_resultssound", S_cvBoole},
   {S_empty},
   {S_label, "st_labl_gui_language"},
   {S_enume, "gui_jpfont", S_cvInteg, S_bndi(0, font_num), "jpfont"},
};

struct setting const st_hud[] = {
   {S_label, "st_labl_hud_personal"},
   {S_boole, "hud_showlvl",   S_cvBoole},
   {S_boole, "hud_showscore", S_cvBoole},
   {S_enume, "hud_expbar",    S_cvInteg, S_bndi(0, lxb_max), "expbar"},
   {S_empty},
   {S_label, "st_labl_hud_hud"},
   {S_enume, "hud_type",         S_cvInteg, S_bndi(0, _hud_max + 1), "hudtype"},
   {S_boole, "hud_showarmorind", S_cvBoole, .pclass = pM},
   {S_boole, "hud_showweapons",  S_cvBoole},
   {S_empty},
   {S_label, "st_labl_hud_log"},
   {S_boole, "hud_showlog",         S_cvBoole},
   {S_fixed, "hud_logsize",         S_cvFixed, S_bndk(0.2, 1.0), "mult"},
   {S_boole, "hud_logfromtop",      S_cvBoole},
   {S_enume, "hud_logcolor",        S_cvInteg, S_color},
   {S_boole, "player_sillypickups", S_cvBoole},
   {S_empty},
   {S_label, "st_labl_hud_pops"},
   {S_enume, "player_scoredisp",     S_cvInteg, S_bndi(_itm_disp_none, _itm_disp_max), "itmdisp"},
   {S_enume, "player_itemdisp",      S_cvInteg, S_bndi(_itm_disp_none, _itm_disp_max), "itmdisp"},
   {S_enume, "player_ammodisp",      S_cvInteg, S_bndi(_itm_disp_none, _itm_disp_max), "itmdisp"},
   {S_boole, "hud_showdamage",       S_cvBoole},
   {S_fixed, "player_itemdispalpha", S_cvFixed, S_bndk(0.0, 1.0), "mult"},
   {S_fixed, "hud_damagealpha",      S_cvFixed, S_bndk(0.0, 1.0), "mult"},
   {S_empty},
   {S_label, "st_labl_hud_nearby"},
   {S_boole, "hud_showitems", S_cvBoole},
   {S_enume, "hud_itemcolor", S_cvInteg, S_color},
   {S_empty},
   {S_label, "st_labl_hud_xhair"},
   {S_boole, "xhair_enable",       S_cvBoole},
   {S_boole, "xhair_enablejuicer", S_cvBoole},
   {S_enume, "xhair_style",        S_cvInteg, S_bndi(0, lxh_max), "xhair"},
   {S_integ, "xhair_r", S_cvInteg, S_bndi(0, 255), "byte"},
   {S_integ, "xhair_g", S_cvInteg, S_bndi(0, 255), "byte"},
   {S_integ, "xhair_b", S_cvInteg, S_bndi(0, 255), "byte"},
   {S_integ, "xhair_a", S_cvInteg, S_bndi(0, 255), "byte"},
   {S_empty},
   {S_label, "st_labl_hud_scanner"},
   {S_integ, "scanner_xoffs", S_cvInteg, S_bndi(-160, 160), "pxls"},
   {S_integ, "scanner_yoffs", S_cvInteg, S_bndi(-160, 160), "pxls"},
   {S_enume, "scanner_font",    S_cvInteg, S_bndi(0, _sfont_max), "sfont"},
   {S_boole, "scanner_bar",     S_cvBoole},
   {S_enume, "scanner_slide",   S_cvInteg, S_bndi(0, _ssld_max), "slide"},
   {S_enume, "scanner_color",   S_cvInteg, S_color},
};

struct setting const st_itm[] = {
   {S_label, "st_labl_itm_autogroups"},
   {S_boole, "st_autobuy_1", .cb_g = {.b = SG_autoBuy}},
   {S_boole, "st_autobuy_2", .cb_g = {.b = SG_autoBuy}},
   {S_boole, "st_autobuy_3", .cb_g = {.b = SG_autoBuy}},
   {S_boole, "st_autobuy_4", .cb_g = {.b = SG_autoBuy}},
   {S_empty},
   {S_label, "st_labl_itm_pickups"},
   {S_boole, "player_brightweps", S_cvBoole},
   {S_boole, "player_noitemfx",   S_cvBoole},
   {S_empty},
   {S_label, "st_labl_itm_balance"},
   {S_boole, "sv_nobossdrop", S_cvBoole},
   {S_boole, "sv_wepdrop",    S_cvBoole},
   {S_empty},
   {S_label, "st_labl_itm_misc"},
   {S_boole, "player_altinvuln",  S_cvBoole},
   {S_boole, "player_scoresound", S_cvBoole},
   {S_boole, "player_teleshop",   S_cvBoole},
   {S_empty},
   {S_label, "st_labl_itm_flashlight"},
   {S_integ, "light_battery", S_cvInteg, S_bndi(0, 60), "secs"},
   {S_integ, "light_regen",   S_cvInteg, S_bndi(1, 10), "mult"},
   {S_integ, "light_r", S_cvInteg, S_bndi(0, 255), "byte"},
   {S_integ, "light_g", S_cvInteg, S_bndi(0, 255), "byte"},
   {S_integ, "light_b", S_cvInteg, S_bndi(0, 255), "byte"},
   {S_integ, "light_radius", S_cvInteg, S_bndi(100, 1000), "unit"},
};

struct setting const st_ply[] = {
   {S_label, "st_labl_ply_balance"},
   {S_integ, "sv_difficulty", S_cvInteg, S_bndi(1,   100),  "perc"},
   {S_integ, "sv_minhealth",  S_cvInteg, S_bndi(0,   200),  "perc"},
   {S_integ, "sv_autosave",   S_cvInteg, S_bndi(0,   30),   "minu"},
   {S_enume, "player_lvsys",  S_cvInteg, S_bndi(0, atsys_max), "lvsys"},
   {S_boole, "sv_extrahard",  S_cvBoole},
   {S_empty},
   {S_label, "st_labl_ply_damage"},
   {S_boole, "sv_revenge",          S_cvBoole},
   {S_boole, "player_damagebob",    S_cvBoole},
   {S_fixed, "player_damagebobmul", S_cvFixed, S_bndk(0.0, 1.0), "mult"},
   {S_empty},
   {S_label, "st_labl_ply_visuals"},
   {S_fixed, "player_footstepvol", S_cvFixed, S_bndk(0.0, 1.0), "mult"},
   {S_fixed, "player_viewtilt",    S_cvFixed, S_bndk(0.0, 1.0), "mult"},
   {S_boole, "st_done_intro",      .cb_g = {.b = SG_doneIntro}},
   {S_empty},
   {S_strng, "player_pronouns", S_cvStrng},
   {S_label, "st_labl_pro_1"},
   {S_label, "st_labl_pro_2"},
   {S_label, "st_labl_pro_3"},
   {S_label, "st_labl_pro_4"},
   {S_label, "st_labl_pro_5"},
   {S_empty},
   {S_boole, "player_bosstexts", S_cvBoole},
   {S_label, "st_labl_boss_1"},
   {S_label, "st_labl_boss_2"},
   {S_label, "st_labl_boss_3"},
   {S_label, "st_labl_boss_4"},
   {S_empty},
   {S_label, "st_labl_ply_postgame"},
   {S_boole, "sv_postgame", S_cvBoole, .fill = true},
   {S_label, "st_labl_postgame_1"},
   {S_label, "st_labl_postgame_2"},
   {S_label, "st_labl_postgame_3"},
   {S_label, "st_labl_postgame_4"},
};

struct setting const st_wep[] = {
   {S_label, "st_labl_wep_effects"},
   {S_boole, "weapons_magicselanims",  S_cvBoole, .pclass = pC},
   {S_boole, "weapons_fastlazshot",    S_cvBoole, .pclass = pM},
   {S_boole, "weapons_rainbowlaser",   S_cvBoole, .pclass = pM},
   {S_boole, "weapons_reducedsg",      S_cvBoole, .pclass = pM},
   {S_boole, "weapons_riflescope",     S_cvBoole, .pclass = pM},
   {S_boole, "weapons_casingfadeout", S_cvBoole},
   {S_boole, "weapons_casings",       S_cvBoole},
   {S_boole, "weapons_magdrops",      S_cvBoole},
   {S_boole, "weapons_magfadeout",    S_cvBoole},
   {S_boole, "weapons_nofirebob",     S_cvBoole},
   {S_fixed, "weapons_alpha",         S_cvFixed, S_bndk(0.0,  1.0), "mult"},
   {S_fixed, "weapons_recoil",        S_cvFixed, S_bndk(0.0,  1.0), "mult"},
   {S_fixed, "weapons_reloadbob",     S_cvFixed, S_bndk(0.0,  1.0), "mult"},
   {S_fixed, "weapons_ricochetvol",   S_cvFixed, S_bndk(0.0,  1.0), "mult"},
   {S_fixed, "weapons_scopealpha",    S_cvFixed, S_bndk(0.0,  1.0), "mult"},
   {S_fixed, "weapons_zoomfactor",    S_cvFixed, S_bndk(1.0, 10.0), "mult"},
   {S_empty},
   {S_label, "st_labl_wep_behaviour"},
   {S_boole, "weapons_riflemodeclear", S_cvBoole, .pclass = pM},
   {S_boole, "weapons_reloadempty",   S_cvBoole},
};

struct setting const st_wld[] = {
   {S_label, "st_labl_wld_balance"},
   {S_boole, "sv_nobosses", S_cvBoole},
   {S_empty},
   {S_label, "st_labl_wld_visuals"},
   {S_boole, "sv_lessparticles", S_cvBoole},
   {S_empty},
   {S_label, "st_labl_wld_rain"},
   {S_boole, "sv_rain",           S_cvBoole},
   {S_boole, "player_rainshader", S_cvBoole},
   {S_empty},
   {S_label, "st_labl_wld_sky"},
   {S_boole, "sv_sky",          S_cvBoole},
   {S_fixed, "sv_skydarkening", S_cvFixed, S_bndk(0.0, 1.0),  "mult"},
};

struct {
   str                  *nam;
   struct setting const *set;
   mem_size_t            num;
} const settings[] = {
#define Typ(name) {&sl_st_labl_##name, st_##name, countof(st_##name)}
   Typ(gui),
   Typ(hud),
   Typ(itm),
   Typ(ply),
   Typ(wep),
   Typ(wld),
#undef Typ
};

void P_CBI_TabSettings(struct gui_state *g) {
   i32 set_num = 0;

   gtab_t tn[countof(settings)];

   for(i32 i = 0; i < countof(settings); i++)
      faststrcpy_str(tn[i], ns(lang(*settings[i].nam)));

   i32 yp = G_Tabs(g, &CBIState(g)->settingstab, tn, countof(tn), 0, 0, 1);
   yp *= gui_p.btntab.h;

   struct setting const *set = settings[CBIState(g)->settingstab].set;
   mem_size_t            num = settings[CBIState(g)->settingstab].num;

   for(i32 i = 0; i < num; i++)
      if(S_isEnabled(&set[i]))
         set_num++;

   G_ScrBeg(g, &CBIState(g)->settingscr, 2, 17 + yp, _rght, 192 - yp,
            set_num * 10);

   struct set_parm sp = {g, 0, 0};

   for(i32 i = 0; i < num; i++) {
      sp.st = &set[i];

      if(S_isEnabled(sp.st)) {
         if(!G_ScrOcc(g, &CBIState(g)->settingscr, sp.y, 10))
            sp.st->cb(&sp);

         sp.y += 10;
      }
   }

   G_ScrEnd(g, &CBIState(g)->settingscr);
}

/* EOF */
