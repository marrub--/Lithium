// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifdef FromUI
Category("Gameplay");
Enum("Level-up system", "lith_player_lvsys", atsys_auto, atsys_manual, "%S", LvSysName(set));
if(p->num == 0) {
   ServerInt("Difficulty Base",      "%", "lith_sv_difficulty", 1, 100);
   ServerFloat("Score multiplier",   "x", "lith_sv_scoremul",   0, 10);
   ServerBool("Explode on death",         "lith_sv_revenge");
   ServerInt("Autosave Interval",  "min", "lith_sv_autosave",   0, 30);
   ServerBool("Disable phantoms",                       "lith_sv_nobosses");
   ServerBool("Don't pick up ammo when full",           "lith_sv_nofullammo");
   ServerBool("Don't give score when picking up ammo",  "lith_sv_noscoreammo");
   ServerBool("Drop shotguns from zombies",             "lith_sv_wepdrop");
   ServerBool("Pause while in menu",                    "lith_sv_pauseinmenus");
}

Category("GUI");
Float("Horizontal cursor speed", "x", "lith_gui_xmul", 0.1, 2.0);
Float("Vertical cursor speed",   "x", "lith_gui_ymul", 0.1, 2.0);
Enum("Color theme",                   "lith_gui_theme",  0, cbi_theme_max-1, "%S", Lith_ThemeName(set));
Enum("Cursor",                        "lith_gui_cursor", 0,  gui_curs_max-1, "%S", CursName(set));

Category("Player");
Float("Damage bob multiplier",       "x", "lith_player_damagebobmul", 0.0, 1.0);
Bool("Bob view when damaged",             "lith_player_damagebob");
Float("Footstep volume",             "x", "lith_player_footstepvol",  0.0, 1.0);
Float("View tilt",                   "x", "lith_player_viewtilt",     0.0, 1.0);
Bool("Log score gained",                  "lith_player_scorelog");
Bool("Play a sound when score is gained", "lith_player_scoresound");
Bool("Play sounds on the results screen", "lith_player_resultssound");
Bool("Invert mouse in netgames (hack)",   "lith_player_invertmouse");
Bool("Enable rain shader",                "lith_player_rainshader");
Bool("Alternate invulnerability palette", "lith_player_altinvuln");

Category("Items");
Bool("Teleport in bought items and upgrades", "lith_player_teleshop");
Bool("Stupid pickup messages",                "lith_player_stupidpickups");
Bool("Log ammo pickups",                      "lith_player_ammolog");
if(p->num == 0) {
   ServerBool("Bright weapon pickups", "lith_player_brightweps");
   ServerBool("No item effects",       "lith_player_noitemfx");
}

Category("Weapons");
Float("Scope zoom factor",  "x", "lith_weapons_zoomfactor", 1.0, 10.0);
Float("Scope opacity",      "x", "lith_weapons_scopealpha", 0.0, 1.0);
Float("Weapon opacity",     "x", "lith_weapons_alpha",      0.0, 1.0);
Float("Recoil amount",      "x", "lith_weapons_recoil",     0.0, 1.0);
Bool("Slot 3 weapons take ammo", "lith_weapons_slot3ammo");
if(p->pclass == pcl_marine) {
   Bool("Modal Rifle scope",          "lith_weapons_riflescope");
   Bool("Clear rifle mode on switch", "lith_weapons_riflemodeclear");
} else if(p->pclass == pcl_cybermage) {
   Bool("Magic selection animations", "lith_weapons_magicselanims");
}
if(p->num == 0) {
   ServerBool("Emit casings from weapons",   "lith_weapons_casings");
   ServerBool("Drop magazines from weapons", "lith_weapons_magdrops");
   ServerBool("Casings fade out",            "lith_weapons_casingfadeout");
   ServerBool("Magazines fade out",          "lith_weapons_magfadeout");
   if(p->pclass == pcl_marine) {
      ServerBool("Rainbow lasers", "lith_weapons_rainbowlaser");
   }
}

Category("Heads Up Display");
Bool("Show score",                    "lith_hud_showscore");
Bool("Show level",                    "lith_hud_showlvl");
Bool("Show weapons",                  "lith_hud_showweapons");
Bool("Show log",                      "lith_hud_showlog");
Bool("Draw log from top of screen",   "lith_hud_logfromtop");
Bool("Large log",                     "lith_hud_logbig");
Bool("Draw reactive armor indicator", "lith_hud_showarmorind");

Int("Crosshair red",     "/255", "lith_xhair_r",     0, 255);
Int("Crosshair green",   "/255", "lith_xhair_g",     0, 255);
Int("Crosshair blue",    "/255", "lith_xhair_b",     0, 255);
Int("Crosshair alpha",   "/255", "lith_xhair_a",     0, 255);
Enum("Crosshair style",          "lith_xhair_style", 1,  10, "%S", XHairName(set));
Bool("Crosshair enabled",        "lith_xhair_enable");
Bool("Crosshair juicer enabled", "lith_xhair_enablejuicer");

Category("Vital Scanner");
Int("X offset",   "px", "lith_scanner_xoffs", -160, 160);
Int("Y offset",   "px", "lith_scanner_yoffs", -180,  20);
Enum("Color",           "lith_scanner_color", 'a', 'v', "\C%c%S", set, ColorName(set));
Bool("Slide to target", "lith_scanner_slide");
Bool("Health bar",      "lith_scanner_bar");
Bool("Alternate font",  "lith_scanner_altfont");

if(p->num == 0) {
   Category("World");
   ServerBool("Rain in outside areas", "lith_sv_rain");
   ServerBool("Replace skies",         "lith_sv_sky");
}

#undef Category
#undef Bool
#undef Float
#undef Int
#undef ServerBool
#undef ServerFloat
#undef ServerInt
#undef Enum
#undef CBox
#undef FromUI

#else

#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

#include <ctype.h>

// Static Functions ----------------------------------------------------------|

static __str LvSysName(int num)
{
   static __str const names[] = {"Auto", "Hybrid", "Manual"};

   if(num < atsys_auto || num > atsys_manual) return "Unknown";
   else                                       return names[num];
}

static __str ColorName(char ch)
{
   static __str const colors[] = {
      "Brick", "Tan", "Grey", "Green", "Brown", "Gold", "Red", "Blue",
      "Orange", "White", "Yellow", "Default", "Black", "Light Blue", "Cream",
      "Olive", "Dark Green", "Dark Red", "Dark Brown", "Purple", "Dark Grey",
      "Cyan"
   };

   if(ch < 'a' || ch > 'v') return "Unknown";
   else                     return colors[ch - 'a'];
}

static __str CursName(int num)
{
   static __str const cursors[] = {
      "Green", "Pink", "Blue", "Orange", "Red", "White", "Outline",
      "Outline (Tail)", "Inv. Outline", "Inv. Outline (Tail)"
   };

   if(num < 0 || num >= gui_curs_max) return "Unknown";
   else                               return cursors[num];
}

static __str XHairName(int num)
{
   static __str const xhairs[] = {
      "Cross", "Circle", "Delta", "Oval", "Basic", "Delear", "Finirentur",
      "Angle", "Dot", "X+"
   };

   if(num < 1 || num > countof(xhairs)) return "Unknown";
   else                                 return xhairs[num - 1];
}

// Extern Functions ----------------------------------------------------------|

void Lith_CBITab_Settings(gui_state_t *g, struct player *p)
{
   int y = 0;

#define Category(...) y += 20
#define Bool(...) y += 10
#define Float(...) y += 10
#define Int(...) y += 10
#define ServerBool(...) y += 10
#define ServerFloat(...) y += 10
#define ServerInt(...) y += 10
#define Enum(...) y += 10
#define CBox(...) y += 10
#define FromUI
#include "p_settings.c"

   Lith_GUI_ScrollBegin(g, &CBIState(g)->settingscr, 15, 30, 280, 192, y);

   y = 0;

#define Label(label) \
   do { \
      PrintTextStr(label); \
      PrintText("cbifont", CR_WHITE, g->ox + 2,1, g->oy + y + 0,1); \
   } while(0)

#define Category(name) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 20)) \
      { \
         PrintTextStr(name); \
         PrintText("chfont", CR_LIGHTBLUE, g->ox + 140,0, g->oy + y + 5,1); \
      } \
      y += 20; \
   } while(0)

#define Bool(label, cvar) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(bool on = p->getCVarI(cvar);) \
      { \
         Label(label); \
         if(Lith_GUI_Button(g, on ? "On" : "Off", 280 - guipre.btnlist.w, y, Pre(btnlist))) \
            p->setCVarI(cvar, !on); \
      } \
      y += 10; \
   } while(0)

#define Float(label, s, cvar, minima, maxima) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(fixed64 set = p->getCVarK(cvar), diff;) \
      { \
         Label(label); \
         if((diff = Lith_GUI_Slider(g, 280 - guipre.slddef.w, y, minima, maxima, set, .suf = s))) \
            p->setCVarK(cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define Int(label, s, cvar, minima, maxima) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(int set = p->getCVarI(cvar), diff;) \
      { \
         Label(label); \
         if((diff = Lith_GUI_Slider(g, 280 - guipre.slddef.w, y, minima, maxima, set, true, .suf = s))) \
            p->setCVarI(cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define ServerBool(label, cvar) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(bool on = ACS_GetCVar(cvar);) \
      { \
         Label(label); \
         if(Lith_GUI_Button(g, on ? "On" : "Off", 280 - guipre.btnlist.w, y, Pre(btnlist))) \
            ACS_SetCVar(cvar, !on); \
      } \
      y += 10; \
   } while(0)

#define ServerFloat(label, s, cvar, minima, maxima) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(fixed64 set = ACS_GetCVarFixed(cvar), diff;) \
      { \
         Label(label); \
         if((diff = Lith_GUI_Slider(g, 280 - guipre.slddef.w, y, minima, maxima, set, .suf = s))) \
            ACS_SetCVarFixed(cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define ServerInt(label, s, cvar, minima, maxima) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(int set = ACS_GetCVar(cvar), diff;) \
      { \
         Label(label); \
         if((diff = Lith_GUI_Slider(g, 280 - guipre.slddef.w, y, minima, maxima, set, true, .suf = s))) \
            ACS_SetCVar(cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define Enum(label, cvar, minima, maxima, fmt, ...) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(int set = p->getCVarI(cvar);) \
      { \
         Label(label); \
         if(Lith_GUI_Button_Id(g, 0, .x = 280 - (guipre.btnnexts.w*2), y, set == minima, Pre(btnprevs))) \
            p->setCVarI(cvar, set - 1); \
         if(Lith_GUI_Button_Id(g, 1, .x = 280 -  guipre.btnnexts.w   , y, set == maxima, Pre(btnnexts))) \
            p->setCVarI(cvar, set + 1); \
         PrintTextFmt(fmt, __VA_ARGS__); \
         PrintText("cbifont", CR_WHITE, g->ox + 200,1, g->oy + y + 0,1); \
      } \
      y += 10; \
   } while(0)

#define CBox(label, on, ...) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
      { \
         Label(label); \
         if(Lith_GUI_Checkbox(g, on, 240, y + 5, Pre(cbxsmall))) \
            (__VA_ARGS__); \
      } \
      y += 10; \
   } while(0)

#define FromUI
#include "p_settings.c"
#undef Label

   Lith_GUI_ScrollEnd(g, &CBIState(g)->settingscr);
}

#endif

// EOF

