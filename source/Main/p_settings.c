/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Settings configuration UI.
 *
 * ---------------------------------------------------------------------------|
 */

#ifdef FromUI
Category(stx_gameplay);
Enum(player_lvsys, atsys_auto, atsys_manual, "%s", LvSysName(set));
if(p->num == 0) {
   ServerInt("%",   sv_difficulty, 1, 100);
   ServerFloat("x", sv_scoremul,   0, 10);
   ServerBool(sv_revenge);
   ServerInt("min", sv_autosave, 0, 30);
   ServerBool(sv_nofullammo);
   ServerBool(sv_noscoreammo);
   ServerBool(sv_wepdrop);
   ServerBool(sv_pauseinmenus);
   ServerBool(sv_nobosses);
   ServerBool(sv_nobossdrop);
}

Category(stx_gui);
Float("x", gui_xmul, 0.1, 2.0);
Float("x", gui_ymul, 0.1, 2.0);
Enum(gui_theme,  0, cbi_theme_max-1, "%s", ThemeName(set));
Enum(gui_cursor, 0, gui_curs_max-1, "%s", CursName(set));
Enum(gui_jpfont, 0, font_num-1, "%s", FontName(set));
Text(stx_jp_0);
Text(stx_jp_1);
Text(stx_jp_2);
Text(stx_jp_3);

Category(stx_player);
Float("x", player_damagebobmul, 0.0, 1.0);
Bool(player_damagebob);
Float("x", player_footstepvol, 0.0, 1.0);
Float("x", player_viewtilt,    0.0, 1.0);
Bool(player_scorelog);
Bool(player_scoresound);
Bool(player_resultssound);
Bool(player_invertmouse);
Bool(player_rainshader);
Bool(player_altinvuln);
Bool(player_bosstexts);
Text(stx_boss_0);
Text(stx_boss_1);
Text(stx_boss_2);
Text(stx_boss_3);

Category(stx_items);
Bool(player_teleshop);
Bool(player_stupidpickups);
Bool(player_ammolog);
if(p->num == 0) {
   ServerBool(player_brightweps);
   ServerBool(player_noitemfx);
}

Category(stx_weapons);
Float("x", weapons_zoomfactor, 1.0, 10.0);
Float("x", weapons_scopealpha, 0.0, 1.0);
Float("x", weapons_alpha,      0.0, 1.0);
Float("x", weapons_recoil,     0.0, 1.0);
Float("x", weapons_reloadbob,  0.0, 1.0);
Bool(weapons_slot3ammo);
if(p->pclass == pcl_marine) {
   Bool(weapons_riflescope);
   Bool(weapons_riflemodeclear);
} else if(p->pclass == pcl_cybermage) {
   Bool(weapons_magicselanims);
}
if(p->num == 0) {
   ServerFloat("x", weapons_ricochetvol, 0.0, 1.0);
   ServerBool(weapons_casings);
   ServerBool(weapons_magdrops);
   ServerBool(weapons_casingfadeout);
   ServerBool(weapons_magfadeout);
   ServerBool(weapons_reloadempty);
   ServerBool(weapons_nofirebob);
   if(p->pclass == pcl_marine) {
      ServerBool(weapons_rainbowlaser);
      ServerBool(weapons_fastlazshot);
   }
}

Category(stx_hud);
Bool(hud_showscore);
Bool(hud_showlvl);
Bool(hud_showweapons);
Bool(hud_showlog);
Bool(hud_logfromtop);
Bool(hud_logbig);
Bool(hud_showarmorind);

Int("/255", xhair_r, 0, 255);
Int("/255", xhair_g, 0, 255);
Int("/255", xhair_b, 0, 255);
Int("/255", xhair_a, 0, 255);
Enum(xhair_style, 1, 10, "%s", XHairName(set));
Bool(xhair_enable);
Bool(xhair_enablejuicer);

Category(stx_vscan);
Int("px", scanner_xoffs, -160, 160);
Int("px", scanner_yoffs, -180,  20);
Enum(scanner_color, 'a', 'v', "\C%c%s", set, ColorName(set));
Bool(scanner_slide);
Bool(scanner_bar);
Bool(scanner_altfont);

if(p->num == 0) {
   Category(stx_world);
   ServerBool(sv_rain);
   ServerBool(sv_sky);
}

#undef Category
#undef Bool
#undef Float
#undef Int
#undef ServerBool
#undef ServerFloat
#undef ServerInt
#undef Enum
#undef Text
#undef FromUI

#else

#include "common.h"
#include "p_player.h"
#include "w_world.h"

/* Static Functions -------------------------------------------------------- */

static cstr LvSysName(i32 num)
{
   static cstr names[] = {"Auto", "Hybrid", "Manual"};

   if(num < atsys_auto || num > atsys_manual) return "Unknown";
   else                                       return names[num];
}

static cstr ColorName(char ch)
{
   static cstr colors[] = {
      "Brick", "Tan", "Grey", "Green", "Brown", "Gold", "Red", "Blue",
      "Orange", "White", "Yellow", "Default", "Black", "Light Blue", "Cream",
      "Olive", "Dark Green", "Dark Red", "Dark Brown", "Purple", "Dark Grey",
      "Cyan"
   };

   if(ch < 'a' || ch > 'v') return "Unknown";
   else                     return colors[ch - 'a'];
}

static cstr CursName(i32 num)
{
   static cstr cursors[] = {
      "Green", "Pink", "Blue", "Orange", "Red", "White", "Outline",
      "Outline (Tail)", "Inv. Outline", "Inv. Outline (Tail)"
   };

   if(num < 0 || num >= gui_curs_max) return "Unknown";
   else                               return cursors[num];
}

static cstr XHairName(i32 num)
{
   static cstr xhairs[] = {
      "Cross", "Circle", "Delta", "Oval", "Basic", "Delear", "Finirentur",
      "Angle", "Dot", "X+"
   };

   if(num < 1 || num > countof(xhairs)) return "Unknown";
   else                                 return xhairs[num - 1];
}

static cstr FontName(i32 num)
{
   static cstr fonts[] = {
      "Misaki Gothic", "Misaki Mincho", "JF Dot k6x8", "JF Dot jiskan"
   };

   if(num < 0 || num >= font_num) return "Unknown";
   else                           return fonts[num];
}

/* Extern Functions -------------------------------------------------------- */

void P_CBI_TabSettings(struct gui_state *g, struct player *p)
{
   i32 y = 0;

#define Category(...) y += 20
#define Bool(...) y += 10
#define Float(...) y += 10
#define Int(...) y += 10
#define ServerBool(...) y += 10
#define ServerFloat(...) y += 10
#define ServerInt(...) y += 10
#define Enum(...) y += 10
#define Text(...) y += 10
#define FromUI
#include "p_settings.c"

   G_ScrollBegin(g, &CBIState(g)->settingscr, 15, 30, 280, 192, y);

   y = 0;

#define Label(label) \
   PrintText_str(L(st_##label), s_smallfnt, CR_WHITE, g->ox + 2,1, g->oy + y + 0,1);

#define Category(name) \
   if(!G_ScrollOcclude(g, &CBIState(g)->settingscr, y, 20)) \
   { \
      PrintText_str(L(st_##name), s_chfont, CR_LIGHTBLUE, g->ox + 140,0, g->oy + y + 5,1); \
   } \
   y += 20;

#define Bool(cvar) \
   do { \
      if(!G_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(bool on = p->getCVarI(sc_##cvar);) \
      { \
         Label(cvar); \
         if(G_Button(g, on ? "On" : "Off", 280 - gui_p.btnlist.w, y, Pre(btnlist))) \
            p->setCVarI(sc_##cvar, !on); \
      } \
      y += 10; \
   } while(0)

#define Float(s, cvar, minima, maxima) \
   do { \
      if(!G_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(k64 set = p->getCVarK(sc_##cvar), diff;) \
      { \
         Label(cvar); \
         if((diff = G_Slider(g, 280 - gui_p.slddef.w, y, minima, maxima, set, .suf = s))) \
            p->setCVarK(sc_##cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define Int(s, cvar, minima, maxima) \
   do { \
      if(!G_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(i32 set = p->getCVarI(sc_##cvar), diff;) \
      { \
         Label(cvar); \
         if((diff = G_Slider(g, 280 - gui_p.slddef.w, y, minima, maxima, set, true, .suf = s))) \
            p->setCVarI(sc_##cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define ServerBool(cvar) \
   do { \
      if(!G_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(bool on = ACS_GetCVar(sc_##cvar);) \
      { \
         Label(cvar); \
         if(G_Button(g, on ? "On" : "Off", 280 - gui_p.btnlist.w, y, Pre(btnlist))) \
            ACS_SetCVar(sc_##cvar, !on); \
      } \
      y += 10; \
   } while(0)

#define ServerFloat(s, cvar, minima, maxima) \
   do { \
      if(!G_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(k64 set = ACS_GetCVarFixed(sc_##cvar), diff;) \
      { \
         Label(cvar); \
         if((diff = G_Slider(g, 280 - gui_p.slddef.w, y, minima, maxima, set, .suf = s))) \
            ACS_SetCVarFixed(sc_##cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define ServerInt(s, cvar, minima, maxima) \
   do { \
      if(!G_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(i32 set = ACS_GetCVar(sc_##cvar), diff;) \
      { \
         Label(cvar); \
         if((diff = G_Slider(g, 280 - gui_p.slddef.w, y, minima, maxima, set, true, .suf = s))) \
            ACS_SetCVar(sc_##cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define Enum(cvar, minima, maxima, fmt, ...) \
   do { \
      if(!G_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(i32 set = p->getCVarI(sc_##cvar);) \
      { \
         Label(cvar); \
         if(G_Button_Id(g, 0, .x = 280 - (gui_p.btnnexts.w*2), y, set == minima, Pre(btnprevs))) \
            p->setCVarI(sc_##cvar, set - 1); \
         if(G_Button_Id(g, 1, .x = 280 -  gui_p.btnnexts.w   , y, set == maxima, Pre(btnnexts))) \
            p->setCVarI(sc_##cvar, set + 1); \
         PrintTextFmt(fmt, __VA_ARGS__); \
         PrintText(s_smallfnt, CR_WHITE, g->ox + 200,1, g->oy + y + 0,1); \
      } \
      y += 10; \
   } while(0)

#define Text(label) \
   do { \
      if(!G_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         Label(label); \
      y += 10; \
   } while(0)

#define FromUI
#include "p_settings.c"
#undef Label

   G_ScrollEnd(g, &CBIState(g)->settingscr);
}

#endif

/* EOF */
