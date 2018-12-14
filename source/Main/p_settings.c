// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifdef FromUI
Category("Gameplay");
Enum("Level-up system", CVAR "player_lvsys", atsys_auto, atsys_manual, "%S", LvSysName(set));
if(p->num == 0) {
   ServerInt("Difficulty Base",     c"%", CVAR "sv_difficulty", 1, 100);
   ServerFloat("Score multiplier",  c"x", CVAR "sv_scoremul",   0, 10);
   ServerBool("Explode on death",         CVAR "sv_revenge");
   ServerInt("Autosave Interval", c"min", CVAR "sv_autosave",   0, 30);
   ServerBool("Disable phantoms",                       CVAR "sv_nobosses");
   ServerBool("Don't pick up ammo when full",           CVAR "sv_nofullammo");
   ServerBool("Don't give score when picking up ammo",  CVAR "sv_noscoreammo");
   ServerBool("Drop shotguns from zombies",             CVAR "sv_wepdrop");
   ServerBool("Pause while in menu",                    CVAR "sv_pauseinmenus");
}

Category("GUI");
Float("Horizontal cursor speed", c"x", CVAR "gui_xmul", 0.1, 2.0);
Float("Vertical cursor speed",   c"x", CVAR "gui_ymul", 0.1, 2.0);
Enum("Color theme",                    CVAR "gui_theme",  0, cbi_theme_max-1, "%S", Lith_ThemeName(set));
Enum("Cursor",                         CVAR "gui_cursor", 0,  gui_curs_max-1, "%S", CursName(set));
Enum("Japanese font",                  CVAR "gui_jpfont", 0, font_num-1, "%S", FontName(set));
Text("To use Japanese language, type \"language jp\" into the console.");

Category("Player");
Float("Damage bob multiplier",      c"x", CVAR "player_damagebobmul", 0.0, 1.0);
Bool("Bob view when damaged",             CVAR "player_damagebob");
Float("Footstep volume",            c"x", CVAR "player_footstepvol",  0.0, 1.0);
Float("View tilt",                  c"x", CVAR "player_viewtilt",     0.0, 1.0);
Bool("Log score gained",                  CVAR "player_scorelog");
Bool("Play a sound when score is gained", CVAR "player_scoresound");
Bool("Play sounds on the results screen", CVAR "player_resultssound");
Bool("Invert mouse in netgames (hack)",   CVAR "player_invertmouse");
Bool("Enable rain shader",                CVAR "player_rainshader");
Bool("Alternate invulnerability palette", CVAR "player_altinvuln");

Category("Items");
Bool("Teleport in bought items and upgrades", CVAR "player_teleshop");
Bool("Stupid pickup messages",                CVAR "player_stupidpickups");
Bool("Log ammo pickups",                      CVAR "player_ammolog");
if(p->num == 0) {
   ServerBool("Bright weapon pickups", CVAR "player_brightweps");
   ServerBool("No item effects",       CVAR "player_noitemfx");
}

Category("Weapons");
Float("Scope zoom factor", c"x", CVAR "weapons_zoomfactor", 1.0, 10.0);
Float("Scope opacity",     c"x", CVAR "weapons_scopealpha", 0.0, 1.0);
Float("Weapon opacity",    c"x", CVAR "weapons_alpha",      0.0, 1.0);
Float("Recoil amount",     c"x", CVAR "weapons_recoil",     0.0, 1.0);
Float("Reload bob",        c"x", CVAR "weapons_reloadbob",  0.0, 1.0);
Bool("Slot 3 weapons take ammo", CVAR "weapons_slot3ammo");
if(p->pclass == pcl_marine) {
   Bool("Modal Rifle scope",          CVAR "weapons_riflescope");
   Bool("Clear rifle mode on switch", CVAR "weapons_riflemodeclear");
} else if(p->pclass == pcl_cybermage) {
   Bool("Magic selection animations", CVAR "weapons_magicselanims");
}
if(p->num == 0) {
   ServerBool("Emit casings from weapons",   CVAR "weapons_casings");
   ServerBool("Drop magazines from weapons", CVAR "weapons_magdrops");
   ServerBool("Casings fade out",            CVAR "weapons_casingfadeout");
   ServerBool("Magazines fade out",          CVAR "weapons_magfadeout");
   if(p->pclass == pcl_marine) {
      ServerBool("Rainbow lasers", CVAR "weapons_rainbowlaser");
   }
}

Category("Heads Up Display");
Bool("Show score",                    CVAR "hud_showscore");
Bool("Show level",                    CVAR "hud_showlvl");
Bool("Show weapons",                  CVAR "hud_showweapons");
Bool("Show log",                      CVAR "hud_showlog");
Bool("Draw log from top of screen",   CVAR "hud_logfromtop");
Bool("Large log",                     CVAR "hud_logbig");
Bool("Draw reactive armor indicator", CVAR "hud_showarmorind");

Int("Crosshair red",    c"/255", CVAR "xhair_r",     0, 255);
Int("Crosshair green",  c"/255", CVAR "xhair_g",     0, 255);
Int("Crosshair blue",   c"/255", CVAR "xhair_b",     0, 255);
Int("Crosshair alpha",  c"/255", CVAR "xhair_a",     0, 255);
Enum("Crosshair style",          CVAR "xhair_style", 1,  10, "%S", XHairName(set));
Bool("Crosshair enabled",        CVAR "xhair_enable");
Bool("Crosshair juicer enabled", CVAR "xhair_enablejuicer");

Category("Vital Scanner");
Int("X offset",  c"px", CVAR "scanner_xoffs", -160, 160);
Int("Y offset",  c"px", CVAR "scanner_yoffs", -180,  20);
Enum("Color",           CVAR "scanner_color", 'a', 'v', "\C%c%S", set, ColorName(set));
Bool("Slide to target", CVAR "scanner_slide");
Bool("Health bar",      CVAR "scanner_bar");
Bool("Alternate font",  CVAR "scanner_altfont");

if(p->num == 0) {
   Category("World");
   ServerBool("Rain in outside areas", CVAR "sv_rain");
   ServerBool("Replace skies",         CVAR "sv_sky");
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
#undef Text
#undef FromUI

#else

#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

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

static __str FontName(int num)
{
   static __str const fonts[] = {
      "Misaki Gothic", "Misaki Mincho", "JF Dot Gothic"
   };

   if(num < 0 || num >= font_num) return "Unknown";
   else                           return fonts[num];
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
#define Text(...) y += 10
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
         if(Lith_GUI_Button(g, on ? c"On" : c"Off", 280 - guipre.btnlist.w, y, Pre(btnlist))) \
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
         if(Lith_GUI_Button(g, on ? c"On" : c"Off", 280 - guipre.btnlist.w, y, Pre(btnlist))) \
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

#define Text(label) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         Label(label); \
      y += 10; \
   } while(0)

#define FromUI
#include "p_settings.c"
#undef Label

   Lith_GUI_ScrollEnd(g, &CBIState(g)->settingscr);
}

#endif

// EOF

