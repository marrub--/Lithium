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

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

enum {
   _left = 2,
   _rght = 280,
   _leng = 10,
};

enum {
   lxh_max = 10,
   lxb_max = 13,
};

enum {
   _s_label,
   _s_boole,
   _s_integ,
   _s_fixed,
   _s_enume,
   _s_strng,
};

/* all members are ordered by how many types use them */
struct setting {
   /* used by all */
   i32 ty;
   /* used by any with text */
   cstr text;
   /* used by any with a scalar */
   union {
      struct {i32 min, max, step;} bi;
      struct {k32 min, max, step;} bk;
   } bnd;
   cstr suff;
   /* used in special cases */
   struct gui_fil *fill;
   i32 pclass;
   union {
      script bool (*cb)(struct set_parm const *sp, bool *v);
      script i32  (*ci)(struct set_parm const *sp, i32  *v);
      script k32  (*ck)(struct set_parm const *sp, k32  *v);
      script str  (*cs)(struct set_parm const *sp, str  *v);
   } f;
};

struct set_parm {
   struct gui_state *g;
   i32 x, y;
   struct setting const *st;
};

struct settings_tab {
   char                  nam[4];
   struct setting const *set;
   mem_size_t            num;
};

#define SG_cvBody(type, name, suff) \
   script static type SG_cv##name(struct set_parm const *sp, type *v) { \
      str cvar = fast_strdup2(CVAR, sp->st->text); \
      if(v) {CVarSet##suff(cvar, *v); return *v;} \
      else  {return CVarGet##suff(cvar);} \
   }

SG_cvBody(bool, Boole, I)
SG_cvBody(i32,  Integ, I)
SG_cvBody(k32,  Fixed, K)
SG_cvBody(str,  Strng, S)

script static bool SG_autoBuy(struct set_parm const *sp, bool *v) {
   i32 which = sp->st->text[faststrlen(sp->st->text) - 1] - '1';
   if(v) {tog_bit(pl.autobuy, which); P_Data_Save();}
   return get_bit(pl.autobuy, which);
}

script static bool SG_doneIntro(struct set_parm const *sp, bool *v) {
   if(v) {tog_bit(pl.done_intro, pl.pclass); P_Data_Save();}
   return get_bit(pl.done_intro, pl.pclass);
}

optargs(1) static void S_label(struct set_parm const *sp, bool tooltip) {
   PrintText_str(ns(lang(fast_strdup2(LANG, sp->st->text))),
                 sf_smallfnt, sp->g->defcr, sp->g->ox + _left,1,
                 sp->g->oy + sp->y,1);
   if(tooltip) {
      G_Tooltip(sp->g, _left, sp->y, _rght - _left, _leng, sp->st->text);
   }
}

static void S_boole(struct set_parm const *sp) {
   bool v = sp->st->f.cb ? sp->st->f.cb(sp, nil) : SG_cvBoole(sp, nil);

   S_label(sp, true);

   if(G_Button_HId(sp->g, sp->y, tmpstr(v ? sl_on : sl_off),
                   _rght - gui_p.btnlist.w, sp->y, Pre(btnlist),
                   .fill = sp->st->fill))
   {
      v = !v;
      sp->st->f.cb ? sp->st->f.cb(sp, &v) : SG_cvBoole(sp, &v);
   }
}

static void S_integ(struct set_parm const *sp) {
   i32 v = sp->st->f.ci ? sp->st->f.ci(sp, nil) : SG_cvInteg(sp, nil);
   S_label(sp, true);
   struct slide_ret sret =
      G_Slider_HId(sp->g, sp->y, _rght - gui_p.slddef.w, sp->y,
                   sp->st->bnd.bi.min, sp->st->bnd.bi.max,
                   sp->st->bnd.bi.step, v,
                   .suf = tmpstr(lang(fast_strdup2(LANG "st_suff_", sp->st->suff))));
   if(sret.different) {
      v = sret.value;
      sp->st->f.ci ? sp->st->f.ci(sp, &v) : SG_cvInteg(sp, &v);
   }
}

static void S_fixed(struct set_parm const *sp) {
   k32 v = sp->st->f.ck ? sp->st->f.ck(sp, nil) : SG_cvFixed(sp, nil);
   S_label(sp, true);
   struct slide_ret sret =
      G_Slider_HId(sp->g, sp->y, _rght - gui_p.slddef.w, sp->y,
                   sp->st->bnd.bk.min, sp->st->bnd.bk.max,
                   sp->st->bnd.bk.step, v,
                   .suf = tmpstr(lang(fast_strdup2(LANG "st_suff_", sp->st->suff))));
   if(sret.different) {
      v = sret.value;
      sp->st->f.ck ? sp->st->f.ck(sp, &v) : SG_cvFixed(sp, &v);
   }
}

static void S_enume(struct set_parm const *sp) {
   i32 v = sp->st->f.ci ? sp->st->f.ci(sp, nil) : SG_cvInteg(sp, nil);
   i32 min = sp->st->bnd.bi.min;
   i32 max = sp->st->bnd.bi.max - 1;
   i32 btw = gui_p.btnnexts.w;

   S_label(sp, true);

   if(G_Button_HId(sp->g, sp->y, .x = _rght - btw * 2, sp->y, v <= min,
                   Pre(btnprevs))) {
      v--;
      sp->st->f.ci ? sp->st->f.ci(sp, &v) : SG_cvInteg(sp, &v);
   }

   if(G_Button_HId(sp->g, sp->y, .x = _rght - btw, sp->y, v >= max,
                   Pre(btnnexts))) {
      v++;
      sp->st->f.ci ? sp->st->f.ci(sp, &v) : SG_cvInteg(sp, &v);
   }

   i32 cr = faststrchk(sp->st->suff, "color") ? Draw_GetCr(v) : sp->g->defcr;

   PrintText_str(ns(v < min || v > max ?
                    sl_st_name_unknown :
                    lang(strp(_l(LANG "st_name_"), _p(sp->st->suff), _c('_'), _p(v)))),
                 sf_smallfnt, cr,
                 sp->g->ox + _rght - btw * 2 - 1,2,
                 sp->g->oy + sp->y,1);
}

static void S_strng(struct set_parm const *sp) {
   str        v   = sp->st->f.cs ? sp->st->f.cs(sp, nil) : SG_cvStrng(sp, nil);
   mem_size_t len = ACS_StrLen(v);

   S_label(sp, true);

   struct gui_txt st;
   st.tbptr = len;
   for(mem_size_t i = 0; i < len; i++) Cps_SetC(st.txtbuf, i, v[i]);

   if(G_TxtBox_HId(sp->g, sp->y, &st, _rght - 130, sp->y)) {
      v = Cps_Expand_str(st.txtbuf, 0, st.tbptr);
      sp->st->f.cs ? sp->st->f.cs(sp, &v) : SG_cvStrng(sp, &v);
   }
}

static bool S_isEnabled(struct setting const *st) {
   return !st->pclass || get_bit(st->pclass, pl.pclass);
}

#define s_label(label, ...)                 {_s_label, label, __VA_ARGS__}
#define s_boole(label, ...)                 {_s_boole, label, __VA_ARGS__}
#define s_integ(label, min, max, step, ...) {_s_integ, label, {.bi = {min, max, step}}, __VA_ARGS__}
#define s_fixed(label, min, max, step, ...) {_s_fixed, label, {.bk = {min, max, step}}, __VA_ARGS__}
#define s_strng(label, ...)                 {_s_strng, label, __VA_ARGS__}
#define s_enume(label, min, max, ...)       {_s_enume, label, {.bi = {min, max, 1}}, __VA_ARGS__}
#define s_color(label, ...)                 s_enume(label, _gcr_first, _gcr_max, "color")

static struct setting const st_gui[] = {
   s_label("st_labl_gui"),
   s_enume("gui_cursor",    0, gui_curs_max, "cursor"),
   s_color("gui_defcr"),
   s_enume("gui_theme",     0, cbi_theme_max, "theme"),
   s_fixed("gui_curspeed",  0.1, 2.0, 0.1, "mult"),
   s_integ("gui_buyfiller", 0, 70, 1, "tick"),
   s_boole("sv_notitleintro"),
   s_label("st_labl_hud"),
   s_enume("hud_type", 0, _hud_max + 1, "hudtype"),
   s_boole("hud_showarmorind", .pclass = pM),
   s_boole("hud_showweapons"),
   s_boole("hud_showlvl"),
   s_boole("hud_showscore"),
   s_enume("hud_expbar", 0, lxb_max, "expbar"),
   s_boole("hud_showlog"),
   s_boole("hud_showammo"),
   s_boole("hud_43aspect"),
   s_fixed("hud_logsize", 0.2, 1.0, 0.05, "mult"),
   s_boole("hud_logfromtop"),
   s_color("hud_logcolor"),
   s_boole("player_sillypickups"),
   s_enume("player_scoredisp", _itm_disp_none, _itm_disp_max, "itmdisp"),
   s_enume("player_itemdisp",  _itm_disp_none, _itm_disp_max, "itmdisp"),
   s_enume("player_ammodisp",  _itm_disp_none, _itm_disp_max, "itmdisp"),
   s_label("st_labl_minimap", .pclass = gI),
   s_fixed("hud_mapscalex",    0.05, 2.0, 0.05, "mult", .pclass = gI),
   s_fixed("hud_mapscaley",    0.05, 2.0, 0.05, "mult", .pclass = gI),
   s_fixed("hud_mapzoom",      0.25, 4.0, 0.05, "mult", .pclass = gI),
   s_fixed("hud_mapthickness", 0.1,  2.0, 0.05, "mult", .pclass = gI),
   s_fixed("hud_mapalpha",     0.1,  1.0, 0.05, "mult", .pclass = gI),
   s_fixed("hud_mapbgalpha",   0.0,  1.0, 0.05, "mult", .pclass = gI),
   s_boole("hud_mapcolor", .pclass = gI),
   s_integ("hud_mapcolor_r", 0, 255, 1, "byte", .pclass = gI),
   s_integ("hud_mapcolor_g", 0, 255, 1, "byte", .pclass = gI),
   s_integ("hud_mapcolor_b", 0, 255, 1, "byte", .pclass = gI),
   s_enume("hud_mapborder", -1, 2, "mapborder", .pclass = gI),
   s_boole("hud_mapbcolor", .pclass = gI),
   s_integ("hud_mapbcolor_r", 0, 255, 1, "byte", .pclass = gI),
   s_integ("hud_mapbcolor_g", 0, 255, 1, "byte", .pclass = gI),
   s_integ("hud_mapbcolor_b", 0, 255, 1, "byte", .pclass = gI),
   s_fixed("hud_mapposx", 0.0, 1.0, 0.05, "mult", .pclass = gI),
   s_fixed("hud_mapposy", 0.0, 1.0, 0.05, "mult", .pclass = gI),
   s_label("st_labl_projected"),
   s_boole("hud_showdamage"),
   s_fixed("player_itemdispalpha", 0.0, 1.0, 0.05, "mult"),
   s_fixed("hud_damagealpha",      0.0, 1.0, 0.05, "mult"),
   s_boole("hud_showitems"),
   s_boole("hud_showinteract"),
   s_color("hud_itemcolor"),
   s_label("st_labl_xhair"),
   s_boole("xhair_enable"),
   s_boole("xhair_enablejuicer"),
   s_enume("xhair_style", 0,    lxh_max,   "xhair"),
   s_integ("xhair_r",     0,    255, 1,    "byte"),
   s_integ("xhair_g",     0,    255, 1,    "byte"),
   s_integ("xhair_b",     0,    255, 1,    "byte"),
   s_integ("xhair_a",     0,    255, 1,    "byte"),
   s_fixed("xhair_scale", 0.25, 4.0, 0.05, "mult"),
   s_label("st_labl_scanner"),
   s_integ("scanner_xoffs", -160, 160, 1, "pxls"),
   s_integ("scanner_yoffs", -160, 160, 1, "pxls"),
   s_enume("scanner_slide", 0, _ssld_max, "slide"),
   s_boole("scanner_bar"),
   s_enume("scanner_font",  0, _sfont_max, "sfont"),
   s_color("scanner_color"),
};

static struct gui_fil fil_boss = {35};
static struct gui_fil fil_postgame = {70};
static struct setting const st_gam[] = {
   s_label("st_labl_balance"),
   s_integ("sv_difficulty", 1, 100, 1, "perc"),
   s_enume("player_lvsys", 0, atsys_max, "lvsys"),
   s_boole("sv_extrahard"),
   s_boole("sv_nobosses", .fill = &fil_boss),
   s_boole("sv_nobossdrop", .fill = &fil_boss),
   s_boole("sv_wepdrop"),
   s_integ("sv_minhealth", 0, 200, 1, "perc"),
   s_integ("sv_autosave",  0, 30, 1, "minu"),
   s_boole("sv_autosavef"),
   s_label("st_labl_fx"),
   s_boole("sv_revenge"),
   s_boole("sv_lessparticles"),
   s_boole("player_altinvuln"),
   s_boole("st_done_intro", .f = {.cb = SG_doneIntro}),
   s_label("st_labl_player"),
   s_boole("player_damagebob"),
   s_fixed("player_damagebobmul", 0.0, 1.0, 0.05, "mult"),
   s_fixed("player_viewtilt",     0.0, 1.0, 0.05, "mult"),
   s_fixed("player_footstepvol",  0.0, 1.0, 0.05, "mult"),
   s_boole("player_resultssound"),
   s_boole("player_scoresound"),
   s_strng("player_pronouns"),
   s_label("st_labl_env"),
   s_integ("sv_timescale", -200, 200, 1, "mult"),
   s_enume("sv_rain",        0, 3, "rain"),
   s_enume("sv_rainphysics", 0, 3, "rainphys"),
   s_boole("player_rainshader"),
   s_enume("sv_sky",          0, 4,           "sky"),
   s_fixed("sv_skydarkening", 0.0, 1.0, 0.05, "mult"),
   s_boole("sv_dynlight"),
   s_boole("sv_dynsky"),
   #ifndef NDEBUG
   s_label("st_labl_postgame"),
   s_boole("sv_postgame", .fill = &fil_postgame),
   #endif
};

static struct setting const st_itm[] = {
   s_label("st_labl_autogroups"),
   s_boole("st_autobuy_1", .f = {.cb = SG_autoBuy}),
   s_boole("st_autobuy_2", .f = {.cb = SG_autoBuy}),
   s_boole("st_autobuy_3", .f = {.cb = SG_autoBuy}),
   s_boole("st_autobuy_4", .f = {.cb = SG_autoBuy}),
   s_label("st_labl_pickups"),
   s_boole("player_brightweps"),
   s_boole("player_noitemfx"),
   s_boole("player_teleshop"),
   s_label("st_labl_flashlight"),
   s_integ("light_battery", 0, 300, 1, "secs"),
   s_integ("light_regen",   1, 10,  1, "mult"),
   s_integ("light_r", 0, 255, 1, "byte"),
   s_integ("light_g", 0, 255, 1, "byte"),
   s_integ("light_b", 0, 255, 1, "byte"),
   s_integ("light_radius", 100, 5000, 100, "unit"),
   s_enume("light_sparksnd", 0, _lspk_snd_max, "lspksnd", .pclass = gI),
};

static struct setting const st_wep[] = {
   s_label("st_labl_effects"),
   s_boole("weapons_magicselanims", .pclass = pC),
   s_boole("weapons_fastlazshot",   .pclass = pM),
   s_boole("weapons_rainbowlaser",  .pclass = pM),
   s_boole("weapons_reducedsg",     .pclass = pM),
   s_boole("weapons_riflescope",    .pclass = pM),
   s_boole("weapons_casingfadeout"),
   s_boole("weapons_casings"),
   s_boole("weapons_magdrops"),
   s_boole("weapons_magfadeout"),
   s_boole("weapons_nofirebob"),
   s_fixed("weapons_recoil",      0.0,  1.0, 0.05, "mult"),
   s_fixed("weapons_reloadbob",   0.0,  1.0, 0.05, "mult"),
   s_fixed("weapons_ricochetvol", 0.0,  1.0, 0.05, "mult"),
   s_fixed("weapons_scopealpha",  0.0,  1.0, 0.05, "mult"),
   s_fixed("weapons_zoomfactor",  1.0, 10.0, 0.1,  "mult"),
   s_label("st_labl_behaviour"),
   s_boole("weapons_riflemodeclear", .pclass = pM),
   s_boole("weapons_reloadempty"),
};

static struct settings_tab const settings[] = {
#define Typ(name) {#name, st_##name, countof(st_##name)}
   Typ(gui),
   Typ(gam),
   Typ(itm),
   Typ(wep),
#undef Typ
};

void P_CBI_TabSettings(struct gui_state *g) {
   noinit static gtab_t tn[countof(settings)];
   i32 set_num = 0;
   for(i32 i = 0; i < countof(settings); i++) {
      faststrcpy_str(tn[i], ns(lang(strp(_l(LANG "st_tab_"), _p((cstr)settings[i].nam)))));
   }
   i32 yp = G_Tabs(g, &pl.cbi.st.settingstab, tn, countof(settings), 1);
   yp *= gui_p.btntab.h;
   i32 y_len = 0;
   struct setting const *set = settings[pl.cbi.st.settingstab].set;
   mem_size_t            num = settings[pl.cbi.st.settingstab].num;
   struct set_parm sp = {g, 0, 0};
   for(i32 i = 0; i < num; i++) {
      sp.st = &set[i];
      if(S_isEnabled(sp.st)) {
         set_num++;
         y_len += _leng;
         if(sp.st->ty == _s_label && i != 0) {
            y_len += _leng;
         }
      }
   }
   G_ScrBeg(g, &pl.cbi.st.settingscr, 2, 17 + yp, _rght, 192 - yp, y_len);
   for(i32 i = 0; i < num; i++) {
      sp.st = &set[i];
      if(S_isEnabled(sp.st)) {
         y_len = _leng;
         if(sp.st->ty == _s_label && i != 0) {
            y_len += _leng;
            sp.y  += _leng;
         }
         if(!G_ScrOcc(g, &pl.cbi.st.settingscr, sp.y, y_len)) {
            switch(sp.st->ty) {
            case _s_label: S_label(&sp); break;
            case _s_boole: S_boole(&sp); break;
            case _s_integ: S_integ(&sp); break;
            case _s_fixed: S_fixed(&sp); break;
            case _s_enume: S_enume(&sp); break;
            case _s_strng: S_strng(&sp); break;
            }
         }
         sp.y += _leng;
      }
   }
   G_ScrEnd(g, &pl.cbi.st.settingscr);
}

/* EOF */
