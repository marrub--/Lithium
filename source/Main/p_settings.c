// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#ifdef FromUI
if(p->num == 0) {
   Category("Gameplay");
   ServerInt("Difficulty Base",    "%", "lith_sv_difficulty", 1, 100);
   ServerFloat("Score multiplier", "x", "lith_sv_scoremul", 0, 10);
   ServerBool("Explode on death",  "lith_sv_revenge");
   ServerInt("Autosave Interval",  "min", "lith_sv_autosave", 0, 30);
}

Category("GUI");
Float("Horizontal cursor speed", "x", "lith_gui_xmul", 0.1f, 2.0f);
Float("Vertical cursor speed",   "x", "lith_gui_ymul", 0.1f, 2.0f);
Enum("Color Theme", "lith_gui_theme", 0, cbi_theme_max-1, "%S", ThemeName(set));
if(world.grafZoneEntered)
   ServerBool("Pause while in menu", "lith_sv_pauseinmenus");

Category("Player");
Bool("Auto-level attributes", "lith_player_autolevel");
Float("Damage bob multiplier", "x", "lith_player_damagebobmul", 0.0f, 1.0f);
Bool("Bob view when damaged",       "lith_player_damagebob");
Float("Footstep volume",       "x", "lith_player_footstepvol", 0.0f, 1.0f);
Float("View tilt",             "x", "lith_player_viewtilt", 0.0f, 1.0f);
Bool("Log score gained",                  "lith_player_scorelog");
Bool("Play a sound when score is gained", "lith_player_scoresound");
Bool("Play sounds on the results screen", "lith_player_resultssound");
Bool("Invert mouse in netgames (hack)",   "lith_player_invertmouse");

Category("Items");
if(p->num == 0) {
   Bool("Bright weapon pickups", "lith_player_brightweps");
   Bool("No item effects",       "lith_player_noitemfx");
}
Bool("Teleport in bought items and upgrades", "lith_player_teleshop");
Bool("Stupid pickup messages", "lith_player_stupidpickups");
Bool("Log ammo pickups",       "lith_player_ammolog");

Category("Weapons");
Float("Scope zoom factor", "x", "lith_weapons_zoomfactor", 1.0f, 10.0f);
Float("Scope opacity",     "x", "lith_weapons_scopealpha", 0.0f, 1.0f);
Float("Weapon opacity",    "x", "lith_weapons_alpha",      0.0f, 1.0f);
Bool("Modal Rifle scope",  "lith_weapons_riflescope");
Bool("Clear rifle mode on switch", "lith_weapons_riflemodeclear");
if(p->num == 0)
   Bool("Drop magazines from weapons", "lith_weapons_magdrops");
Bool("Magic selection animations", "lith_weapons_magicselanims");

Category("Heads Up Display");
Bool("Show score",   "lith_hud_showscore");
Bool("Show weapons", "lith_hud_showweapons");
Bool("Show log",     "lith_hud_showlog");
Bool("Draw log from top of screen", "lith_hud_logfromtop");
Bool("Draw reactive armor indicator", "lith_hud_showarmorind");

Category("Vital Scanner");
Int("X offset",         "px", "lith_scanner_xoffs", -160, 160);
Int("Y offset",         "px", "lith_scanner_yoffs", -180, 20);
Enum("Color", "lith_scanner_color", 'a', 'v', "\C%c%S", set, ColorName(set));
Bool("Slide to target", "lith_scanner_slide");
Bool("Health bar",      "lith_scanner_bar");
Bool("Alternate font",  "lith_scanner_altfont");

if(p->num == 0) {
   Category("World");
   ServerBool("Rain in outside areas", "lith_sv_rain");
   ServerBool("Replace skies",         "lith_sv_sky");

   Category("Game Modes");
   ServerBool("Are you serious?", "lith_sv_seriousmode");
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

//
// ColorName
//
static __str ColorName(char ch)
{
   __str colors[] = {
      "Brick", "Tan", "Grey", "Green", "Brown", "Gold", "Red", "Blue",
      "Orange", "White", "Yellow", "Default", "Black", "Light Blue", "Cream",
      "Olive", "Dark Green", "Dark Red", "Dark Brown", "Purple", "Dark Grey",
      "Cyan"
   };

   if(ch < 'a' || ch > 'v') return "Unknown";
   else                     return colors[ch - 'a'];
}

//
// ThemeName
//
static __str ThemeName(int num)
{
   switch(num)
   {
   case cbi_theme_green:    return "Green";
   case cbi_theme_rose:     return "Rose";
   case cbi_theme_umi:      return "Umi";
   case cbi_theme_ender:    return "Ender";
   case cbi_theme_orange:   return "Orange";
   case cbi_theme_grey:     return "Grey";
   case cbi_theme_bassilla: return "Bassilla";
   case cbi_theme_ghost:    return "Ghost";
   default:                 return "Unknown";
   }
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_CBITab_Settings
//
void Lith_CBITab_Settings(gui_state_t *g, player_t *p)
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

   Lith_GUI_ScrollBegin(g, st_settingscr, 15, 30, 280, 192, y);

   y = 0;

#define Label(label) \
   do { \
      HudMessageF("CBIFONT", label); \
      HudMessagePlain(g->hid--, g->ox + 2.1, g->oy + y + 0.1, TICSECOND); \
   } while(0)

#define Category(name) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 20)) \
      { \
         HudMessageF("CHFONT", "\Cn" name); \
         HudMessagePlain(g->hid--, g->ox + 140.0, g->oy + y + 5.1, TICSECOND); \
      } \
      y += 20; \
   } while(0)

#define Bool(label, cvar) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
         __with(bool on = p->getCVarI(cvar);) \
      { \
         Label(label); \
         if(Lith_GUI_Button(g, on ? "On" : "Off", 280 - guipre.btnlist.w, y, .preset = &guipre.btnlist)) \
            p->setCVarI(cvar, !on); \
      } \
      y += 10; \
   } while(0)

#define Float(label, s, cvar, minima, maxima) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
         __with(double set = p->getCVarK(cvar), diff;) \
      { \
         Label(label); \
         if((diff = Lith_GUI_Slider(g, 280 - guipre.slddef.w, y, minima, maxima, set, .suf = s))) \
            p->setCVarK(cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define Int(label, s, cvar, minima, maxima) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
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
      if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
         __with(bool on = ACS_GetCVar(cvar);) \
      { \
         Label(label); \
         if(Lith_GUI_Button(g, on ? "On" : "Off", 280 - guipre.btnlist.w, y, .preset = &guipre.btnlist)) \
            ACS_SetCVar(cvar, !on); \
      } \
      y += 10; \
   } while(0)

#define ServerFloat(label, s, cvar, minima, maxima) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
         __with(double set = ACS_GetCVarFixed(cvar), diff;) \
      { \
         Label(label); \
         if((diff = Lith_GUI_Slider(g, 280 - guipre.slddef.w, y, minima, maxima, set, .suf = s))) \
            ACS_SetCVarFixed(cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define ServerInt(label, s, cvar, minima, maxima) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
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
      if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
         __with(int set = p->getCVarI(cvar);) \
      { \
         Label(label); \
         if(Lith_GUI_Button_Id(g, 0, .x = 280 - (guipre.btnnexts.w*2), y, set == minima, .preset = &guipre.btnprevs)) \
            p->setCVarI(cvar, set - 1); \
         if(Lith_GUI_Button_Id(g, 1, .x = 280 -  guipre.btnnexts.w   , y, set == maxima, .preset = &guipre.btnnexts)) \
            p->setCVarI(cvar, set + 1); \
         HudMessageF("CBIFONT", fmt, __VA_ARGS__); \
         HudMessagePlain(g->hid--, g->ox + 200.1, g->oy + y + 0.1, TICSECOND); \
      } \
      y += 10; \
   } while(0)

#define CBox(label, on, ...) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
      { \
         Label(label); \
         if(Lith_GUI_Checkbox(g, on, 240, y + 5, .preset = &guipre.cbxsmall)) \
            (__VA_ARGS__); \
      } \
      y += 10; \
   } while(0)

#define FromUI
#include "p_settings.c"
#undef Label

   Lith_GUI_ScrollEnd(g, st_settingscr);
}

#endif

// EOF

