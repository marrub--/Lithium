/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Settings configuration UI.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"

/* Types ------------------------------------------------------------------- */

enum {
   _left = 2,
   _rght = 280,
};

enum {
   lxh_max = 10,
   lxb_max = 13,
};

/* all members are ordered by how many types use them */
struct setting {
   /* used by all */
   script void (*cb)(struct set_parm const *sp);
   /* used by any with text */
   cstr text;
   /* used by any with a variable */
   union {
      script bool (*b)(struct set_parm const *sp, bool *v);
      script i32  (*i)(struct set_parm const *sp, i32  *v);
      script k32  (*k)(struct set_parm const *sp, k32  *v);
   } cb_g;
   script bool (*cb_e)(struct setting const *st, struct player *p);
   /* used by any with a scalar */
   union {
      struct {i32 min, max;} i;
      struct {k32 min, max;} k;
   } bnd;
   cstr suff;
   /* used in special cases */
   u32 fill;
   i32 pclass;
};

struct set_parm {
   struct player    *p;
   struct gui_state *g;
   i32 x, y;
   struct setting const *st;
};

/* Static Functions -------------------------------------------------------- */

#define SG_clBody(type, name, suff) \
   script static \
   type SG_cl##name(struct set_parm const *sp, type *v) { \
      str cvar = l_strcpy2(CVAR, sp->st->text); \
      if(v) {sp->p->set##suff(cvar, *v); return *v;} \
      else  {return sp->p->get##suff(cvar);} \
   }

#define SG_svBody(type, name, suff) \
   script static \
   type SG_sv##name(struct set_parm const *sp, type *v) { \
      str cvar = l_strcpy2(CVAR, sp->st->text); \
      if(v) {ACS_Set##suff(cvar, *v); return *v;} \
      else  {return ACS_Get##suff(cvar);} \
   }

SG_clBody(bool, Boole, CVarI)
SG_clBody(i32,  Integ, CVarI)
SG_clBody(k32,  Fixed, CVarK)

SG_svBody(bool, Boole, CVar)
SG_svBody(i32,  Integ, CVar)
SG_svBody(k32,  Fixed, CVarFixed)

script static
bool SG_autoBuy(struct set_parm const *sp, bool *v) {
   i32 which = sp->st->text[strlen(sp->st->text) - 1] - '1';
   if(v) {tog_bit(sp->p->autobuy, which); P_Data_Save(sp->p);}
   return get_bit(sp->p->autobuy, which);
}

script static
bool SG_doneIntro(struct set_parm const *sp, bool *v) {
   if(v) {sp->p->done_intro ^= sp->p->pclass; P_Data_Save(sp->p);}
   return sp->p->done_intro &  sp->p->pclass;
}

script static
bool SE_server(struct setting const *st, struct player *p) {
   return p->num == 0;
}

script static
void S_empty(struct set_parm const *sp) {
}

script static
void S_label(struct set_parm const *sp) {
   char buf[64]; lstrcpy2(buf, LANG, sp->st->text);
   PrintTextChS(LC(buf));
   PrintText(s_smallfnt, sp->g->defcr,
             sp->g->ox + _left,1, sp->g->oy + sp->y,1);
}

script static
void S_boole(struct set_parm const *sp) {
   bool v = sp->st->cb_g.b(sp, nil);

   S_label(sp);

   if(G_Button_HId(sp->g, sp->y, v ? LC(LANG "ON") : LC(LANG "OFF"),
                   _rght - gui_p.btnlist.w, sp->y, Pre(btnlist),
                   .fill = {&CBIState(sp->g)->settingsfill, sp->st->fill})) {
      v = !v;
      sp->st->cb_g.b(sp, &v);
   }
}

script static
void S_integ(struct set_parm const *sp) {
   char suff[32]; lstrcpy2(suff, LANG "st_suff_", sp->st->suff);
   i32 diff, v = sp->st->cb_g.i(sp, nil);

   S_label(sp);

   if((diff = G_Slider_HId(sp->g, sp->y, _rght - gui_p.slddef.w, sp->y,
                           sp->st->bnd.i.min, sp->st->bnd.i.max, v, true,
                           .suf = LC(suff)))) {
      v += diff;
      sp->st->cb_g.i(sp, &v);
   }
}

script static
void S_fixed(struct set_parm const *sp) {
   char suff[32]; lstrcpy2(suff, LANG "st_suff_", sp->st->suff);
   k32 diff, v = sp->st->cb_g.k(sp, nil);

   S_label(sp);

   if((diff = G_Slider_HId(sp->g, sp->y, _rght - gui_p.slddef.w, sp->y,
                           sp->st->bnd.k.min, sp->st->bnd.k.max, v,
                           .suf = LC(suff)))) {
      v = (i32)((v + diff) * 100) / 100.0k;
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

   i32 cr = !strcmp(sp->st->suff, "color") ? Draw_GetCr(v) : sp->g->defcr;

   if(v < min || v > max)
      PrintTextChS(LC(LANG "st_name_unknown"));
   else
      PrintTextChS(LanguageC(LANG "st_name_%s_%i", sp->st->suff, v));

   PrintText(s_smallfnt, cr,
             sp->g->ox + _rght - btw * 2 - 1,2,
             sp->g->oy + sp->y,1);
}

static
bool S_isEnabled(struct setting const *st, struct player *p) {
   return
      (!st->cb_e   || st->cb_e(st, p)) &&
      (!st->pclass || p->pclass & st->pclass);
}

/* Static Objects ---------------------------------------------------------- */

#define S_bndi(min, max) .bnd = {.i = {min, max}}
#define S_bndk(min, max) .bnd = {.k = {min, max}}

#define S_color S_bndi('a', 'z' + _gcr_max), "color"

#define S_clBoole .cb_g = {.b = SG_clBoole}
#define S_clInteg .cb_g = {.i = SG_clInteg}
#define S_clFixed .cb_g = {.k = SG_clFixed}

#define S_svEnabl .cb_e = SE_server
#define S_svBoole S_svEnabl, .cb_g = {.b = SG_svBoole}
#define S_svInteg S_svEnabl, .cb_g = {.i = SG_svInteg}
#define S_svFixed S_svEnabl, .cb_g = {.k = SG_svFixed}

struct setting const st_gui[] = {
   {S_enume, "gui_cursor",    S_clInteg, S_bndi(0, gui_curs_max), "cursor"},
   {S_enume, "gui_defcr",     S_clInteg, S_color},
   {S_enume, "gui_theme",     S_clInteg, S_bndi(0, cbi_theme_max), "theme"},
   {S_fixed, "gui_xmul",      S_clFixed, S_bndk(0.1, 2.0), "mult"},
   {S_fixed, "gui_ymul",      S_clFixed, S_bndk(0.1, 2.0), "mult"},
   {S_integ, "gui_buyfiller", S_clInteg, S_bndi(0, 70),  "tick"},
   {S_empty},
   {S_enume, "gui_jpfont", S_clInteg, S_bndi(0, font_num), "jpfont"},
   {S_empty},
   {S_label, "st_labl_jp_1"},
   {S_label, "st_labl_jp_2"},
   {S_label, "st_labl_jp_3"},
};

struct setting const st_hud[] = {
   {S_boole, "hud_showarmorind", S_clBoole},
   {S_boole, "hud_showdamage",   S_clBoole},
   {S_boole, "hud_showlvl",      S_clBoole},
   {S_boole, "hud_showscore",    S_clBoole},
   {S_boole, "hud_showweapons",  S_clBoole},
   {S_enume, "hud_expbar",       S_clInteg, S_bndi(0, lxb_max), "expbar"},
   {S_empty},
   {S_boole, "xhair_enable",       S_clBoole},
   {S_boole, "xhair_enablejuicer", S_clBoole},
   {S_enume, "xhair_style",        S_clInteg, S_bndi(0, lxh_max), "xhair"},
   {S_empty},
   {S_integ, "xhair_r", S_clInteg, S_bndi(0, 255), "byte"},
   {S_integ, "xhair_g", S_clInteg, S_bndi(0, 255), "byte"},
   {S_integ, "xhair_b", S_clInteg, S_bndi(0, 255), "byte"},
   {S_empty},
   {S_integ, "scanner_xoffs", S_clInteg, S_bndi(-160, 160), "pxls"},
   {S_integ, "scanner_yoffs", S_clInteg, S_bndi(-160, 160), "pxls"},
   {S_empty},
   {S_boole, "scanner_altfont", S_clBoole},
   {S_boole, "scanner_bar",     S_clBoole},
   {S_boole, "scanner_slide",   S_clBoole},
   {S_enume, "scanner_color",   S_clInteg, S_color},
};

struct setting const st_itm[] = {
   {S_boole, "player_altinvuln",  S_clBoole},
   {S_boole, "player_brightweps", S_svBoole},
   {S_boole, "player_noitemfx",   S_svBoole},
   {S_boole, "player_scoresound", S_clBoole},
   {S_boole, "player_teleshop",   S_clBoole},
   {S_boole, "sv_nobossdrop",     S_svBoole},
   {S_boole, "sv_nofullammo",     S_svBoole},
   {S_boole, "sv_noscoreammo",    S_svBoole},
   {S_boole, "sv_wepdrop",        S_svBoole},
};

struct setting const st_lit[] = {
   {S_integ, "light_battery", S_clInteg, S_bndi(0, 60), "secs"},
   {S_integ, "light_regen",   S_clInteg, S_bndi(1, 10), "mult"},
   {S_empty},
   {S_integ, "light_r", S_clInteg, S_bndi(0, 255), "byte"},
   {S_integ, "light_g", S_clInteg, S_bndi(0, 255), "byte"},
   {S_integ, "light_b", S_clInteg, S_bndi(0, 255), "byte"},
   {S_empty},
   {S_integ, "light_radius", S_clInteg, S_bndi(100, 1000), "unit"},
};

struct setting const st_log[] = {
   {S_boole, "hud_logfromtop",       S_clBoole},
   {S_boole, "hud_showlog",          S_clBoole},
   {S_boole, "player_ammolog",       S_clBoole},
   {S_boole, "player_scorelog",      S_clBoole},
   {S_boole, "player_stupidpickups", S_clBoole},
   {S_fixed, "hud_logsize",          S_clFixed, S_bndk(0.2, 1.0), "mult"},
};

struct setting const st_pgm[] = {
   {S_boole, "sv_postgame", S_svBoole, .fill = 35 * 5},
   {S_empty},
   {S_label, "st_labl_postgame_1"},
   {S_label, "st_labl_postgame_2"},
   {S_label, "st_labl_postgame_3"},
   {S_label, "st_labl_postgame_4"},
};

struct setting const st_ply[] = {
   {S_boole, "st_autobuy_1", .cb_g = {.b = SG_autoBuy}},
   {S_boole, "st_autobuy_2", .cb_g = {.b = SG_autoBuy}},
   {S_boole, "st_autobuy_3", .cb_g = {.b = SG_autoBuy}},
   {S_boole, "st_autobuy_4", .cb_g = {.b = SG_autoBuy}},
   {S_boole, "sv_revenge",   S_svBoole},
   {S_integ, "sv_minhealth", S_svInteg, S_bndi(0,       200), "perc"},
   {S_enume, "player_lvsys", S_svInteg, S_bndi(0, atsys_max), "lvsys"},
   {S_empty},
   {S_boole, "player_damagebob",    S_clBoole},
   {S_fixed, "player_damagebobmul", S_clFixed, S_bndk(0.0, 1.0), "mult"},
   {S_empty},
   {S_boole, "player_invertmouse",  S_clBoole},
   {S_boole, "player_resultssound", S_clBoole},
   {S_boole, "st_done_intro",       .cb_g = {.b = SG_doneIntro}},
   {S_fixed, "player_footstepvol",  S_clFixed, S_bndk(0.0, 1.0), "mult"},
   {S_fixed, "player_viewtilt",     S_clFixed, S_bndk(0.0, 1.0), "mult"},
   {S_empty},
   {S_boole, "player_bosstexts", S_clBoole},
   {S_empty},
   {S_label, "st_labl_boss_1"},
   {S_label, "st_labl_boss_2"},
   {S_label, "st_labl_boss_3"},
};

struct setting const st_wep[] = {
   {S_boole, "weapons_magicselanims", S_clBoole, .pclass = pC},
   {S_empty,                                     .pclass = pC},

   {S_boole, "weapons_fastlazshot",    S_svBoole, .pclass = pM},
   {S_boole, "weapons_rainbowlaser",   S_svBoole, .pclass = pM},
   {S_boole, "weapons_reducedsg",      S_clBoole, .pclass = pM},
   {S_boole, "weapons_riflemodeclear", S_clBoole, .pclass = pM},
   {S_boole, "weapons_riflescope",     S_clBoole, .pclass = pM},
   {S_empty,                                      .pclass = pM},

   {S_boole, "weapons_casingfadeout", S_svBoole},
   {S_boole, "weapons_casings",       S_svBoole},
   {S_boole, "weapons_magdrops",      S_svBoole},
   {S_boole, "weapons_magfadeout",    S_svBoole},
   {S_boole, "weapons_nofirebob",     S_svBoole},
   {S_boole, "weapons_reloadempty",   S_svBoole},
   {S_boole, "weapons_slot3ammo",     S_clBoole},
   {S_fixed, "weapons_alpha",         S_clFixed, S_bndk(0.0,  1.0), "mult"},
   {S_fixed, "weapons_recoil",        S_clFixed, S_bndk(0.0,  1.0), "mult"},
   {S_fixed, "weapons_reloadbob",     S_clFixed, S_bndk(0.0,  1.0), "mult"},
   {S_fixed, "weapons_ricochetvol",   S_svFixed, S_bndk(0.0,  1.0), "mult"},
   {S_fixed, "weapons_scopealpha",    S_clFixed, S_bndk(0.0,  1.0), "mult"},
   {S_fixed, "weapons_zoomfactor",    S_clFixed, S_bndk(1.0, 10.0), "mult"},
};

struct setting const st_wld[] = {
   {S_boole, "player_rainshader", S_clBoole},
   {S_boole, "sv_lessparticles",  S_svBoole},
   {S_boole, "sv_nobosses",       S_svBoole},
   {S_boole, "sv_rain",           S_svBoole},
   {S_boole, "sv_sky",            S_svBoole},
   {S_fixed, "sv_scoremul",       S_svFixed, S_bndk(0.0, 10.0), "mult"},
   {S_integ, "sv_autosave",       S_svInteg, S_bndi(0,   30),   "minu"},
   {S_integ, "sv_difficulty",     S_svInteg, S_bndi(1,   100),  "perc"},
};

struct {
   cstr                  nam;
   struct setting const *set;
   size_t                num;
} const settings[] = {
#define Typ(name) {LANG "st_labl_" #name, st_##name, countof(st_##name)}
   Typ(gui),
   Typ(hud),
   Typ(itm),
   Typ(lit),
   Typ(log),
   Typ(pgm),
   Typ(ply),
   Typ(wep),
   Typ(wld),
#undef Typ
};

/* Extern Functions -------------------------------------------------------- */

void P_CBI_TabSettings(struct gui_state *g, struct player *p) {
   i32 set_num = 0;

   char tn[countof(settings)][20];

   for(i32 i = 0; i < countof(settings); i++)
      LanguageVC(tn[i], settings[i].nam);

   i32 yp = G_Tabs(g, &CBIState(g)->settingstab, tn, countof(tn), 0, 0, 1);
   yp *= gui_p.btntab.h;

   struct setting const *set = settings[CBIState(g)->settingstab].set;
   size_t                num = settings[CBIState(g)->settingstab].num;

   for(i32 i = 0; i < num; i++)
      if(S_isEnabled(&set[i], p))
         set_num++;

   G_ScrBeg(g, &CBIState(g)->settingscr, 2, 17 + yp, _rght, 192 - yp,
            set_num * 10);

   struct set_parm sp = {p, g, 0, 0};

   for(i32 i = 0; i < num; i++) {
      sp.st = &set[i];

      if(S_isEnabled(sp.st, p)) {
         if(!G_ScrOcc(g, &CBIState(g)->settingscr, sp.y, 10))
            sp.st->cb(&sp);

         sp.y += 10;
      }
   }

   G_ScrEnd(g, &CBIState(g)->settingscr);
}

/* EOF */
