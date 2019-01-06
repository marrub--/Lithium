// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_settings.c: Settings configuration UI.

#ifdef FromUI
Category("Gameplay");
Enum("Level-up system", sc_player_lvsys, atsys_auto, atsys_manual, "%s", LvSysName(set));
if(p->num == 0) {
   ServerInt("Difficulty Base",     "%", sc_sv_difficulty, 1, 100);
   ServerFloat("Score multiplier",  "x", sc_sv_scoremul,   0, 10);
   #if LITHIUM
   ServerBool("Explode on death",        sc_sv_revenge);
   #endif
   ServerInt("Autosave Interval", "min", sc_sv_autosave,   0, 30);
   #if LITHIUM
   ServerBool("Disable phantoms",                      sc_sv_nobosses);
   ServerBool("Don't pick up ammo when full",          sc_sv_nofullammo);
   ServerBool("Don't give score when picking up ammo", sc_sv_noscoreammo);
   ServerBool("Drop shotguns from zombies",            sc_sv_wepdrop);
   ServerBool("Pause while in menu",                   sc_sv_pauseinmenus);
   #endif
}

Category("GUI");
Float("Horizontal cursor speed", "x", sc_gui_xmul, 0.1, 2.0);
Float("Vertical cursor speed",   "x", sc_gui_ymul, 0.1, 2.0);
Enum("Color theme",                   sc_gui_theme,  0, cbi_theme_max-1, "%s", Lith_ThemeName(set));
Enum("Cursor",                        sc_gui_cursor, 0, gui_curs_max-1, "%s", CursName(set));
#if LITHIUM
Enum("Japanese font",                 sc_gui_jpfont, 0, font_num-1, "%s", FontName(set));
Text("");
Text("To use Japanese language,");
Text("type \"language jp\" into the console");
Text("and restart the game.");
#endif

Category("Player");
Float("Damage bob multiplier",       "x", sc_player_damagebobmul, 0.0, 1.0);
Bool("Bob view when damaged",             sc_player_damagebob);
Float("Footstep volume",             "x", sc_player_footstepvol,  0.0, 1.0);
Float("View tilt",                   "x", sc_player_viewtilt,     0.0, 1.0);
Bool("Log score gained",                  sc_player_scorelog);
Bool("Play a sound when score is gained", sc_player_scoresound);
#if LITHIUM
Bool("Play sounds on the results screen", sc_player_resultssound);
#endif
Bool("Invert mouse in netgames (hack)",   sc_player_invertmouse);
#if LITHIUM
Bool("Enable rain shader",                sc_player_rainshader);
Bool("Alternate invulnerability palette", sc_player_altinvuln);
#endif

#if LITHIUM
Category("Items");
Bool("Teleport in bought items and upgrades", sc_player_teleshop);
Bool("Stupid pickup messages",                sc_player_stupidpickups);
Bool("Log ammo pickups",                      sc_player_ammolog);
if(p->num == 0) {
   ServerBool("Bright weapon pickups", sc_player_brightweps);
   ServerBool("No item effects",       sc_player_noitemfx);
}

Category("Weapons");
Float("Scope zoom factor", "x", sc_weapons_zoomfactor, 1.0, 10.0);
Float("Scope opacity",     "x", sc_weapons_scopealpha, 0.0, 1.0);
Float("Weapon opacity",    "x", sc_weapons_alpha,      0.0, 1.0);
Float("Recoil amount",     "x", sc_weapons_recoil,     0.0, 1.0);
Float("Reload bob",        "x", sc_weapons_reloadbob,  0.0, 1.0);
Bool("Slot 3 weapons take ammo", sc_weapons_slot3ammo);
if(p->pclass == pcl_marine) {
   Bool("Modal Rifle scope",          sc_weapons_riflescope);
   Bool("Clear rifle mode on switch", sc_weapons_riflemodeclear);
} else if(p->pclass == pcl_cybermage) {
   Bool("Magic selection animations", sc_weapons_magicselanims);
}
if(p->num == 0) {
   ServerFloat("Ricochet volume", "x",       sc_weapons_ricochetvol, 0.0, 1.0);
   ServerBool("Emit casings from weapons",   sc_weapons_casings);
   ServerBool("Drop magazines from weapons", sc_weapons_magdrops);
   ServerBool("Casings fade out",            sc_weapons_casingfadeout);
   ServerBool("Magazines fade out",          sc_weapons_magfadeout);
   ServerBool("Reload empty weapons",        sc_weapons_reloadempty);
   ServerBool("No mid-fire bobbing",         sc_weapons_nofirebob);
   if(p->pclass == pcl_marine) {
      ServerBool("Rainbow lasers", sc_weapons_rainbowlaser);
   }
}

Category("Heads Up Display");
Bool("Show score",                    sc_hud_showscore);
Bool("Show level",                    sc_hud_showlvl);
Bool("Show weapons",                  sc_hud_showweapons);
Bool("Show log",                      sc_hud_showlog);
Bool("Draw log from top of screen",   sc_hud_logfromtop);
Bool("Large log",                     sc_hud_logbig);
Bool("Draw reactive armor indicator", sc_hud_showarmorind);

Int("Crosshair red",     "/255", sc_xhair_r,     0, 255);
Int("Crosshair green",   "/255", sc_xhair_g,     0, 255);
Int("Crosshair blue",    "/255", sc_xhair_b,     0, 255);
Int("Crosshair alpha",   "/255", sc_xhair_a,     0, 255);
Enum("Crosshair style",          sc_xhair_style, 1,  10, "%s", XHairName(set));
Bool("Crosshair enabled",        sc_xhair_enable);
Bool("Crosshair juicer enabled", sc_xhair_enablejuicer);
#endif

Category("Vital Scanner");
Int("X offset",   "px", sc_scanner_xoffs, -160, 160);
Int("Y offset",   "px", sc_scanner_yoffs, -180,  20);
Enum("Color",           sc_scanner_color, 'a', 'v', "\C%c%s", set, ColorName(set));
Bool("Slide to target", sc_scanner_slide);
Bool("Health bar",      sc_scanner_bar);
Bool("Alternate font",  sc_scanner_altfont);

#if LITHIUM
if(p->num == 0) {
   Category("World");
   ServerBool("Rain in outside areas", sc_sv_rain);
   ServerBool("Replace skies",         sc_sv_sky);
}
#endif

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

#include "common.h"
#include "p_player.h"
#include "w_world.h"

// Static Functions ----------------------------------------------------------|

static char const *LvSysName(i32 num)
{
   static char const *names[] = {"Auto", "Hybrid", "Manual"};

   if(num < atsys_auto || num > atsys_manual) return "Unknown";
   else                                       return names[num];
}

static char const *ColorName(char ch)
{
   static char const *colors[] = {
      "Brick", "Tan", "Grey", "Green", "Brown", "Gold", "Red", "Blue",
      "Orange", "White", "Yellow", "Default", "Black", "Light Blue", "Cream",
      "Olive", "Dark Green", "Dark Red", "Dark Brown", "Purple", "Dark Grey",
      "Cyan"
   };

   if(ch < 'a' || ch > 'v') return "Unknown";
   else                     return colors[ch - 'a'];
}

static char const *CursName(i32 num)
{
   static char const *cursors[] = {
      "Green", "Pink", "Blue", "Orange", "Red", "White", "Outline",
      "Outline (Tail)", "Inv. Outline", "Inv. Outline (Tail)"
   };

   if(num < 0 || num >= gui_curs_max) return "Unknown";
   else                               return cursors[num];
}

static char const *XHairName(i32 num)
{
   static char const *xhairs[] = {
      "Cross", "Circle", "Delta", "Oval", "Basic", "Delear", "Finirentur",
      "Angle", "Dot", "X+"
   };

   if(num < 1 || num > countof(xhairs)) return "Unknown";
   else                                 return xhairs[num - 1];
}

static char const *FontName(i32 num)
{
   static char const *fonts[] = {
      "Misaki Gothic", "Misaki Mincho", "JF Dot Gothic"
   };

   if(num < 0 || num >= font_num) return "Unknown";
   else                           return fonts[num];
}

// Extern Functions ----------------------------------------------------------|

void Lith_CBITab_Settings(struct gui_state *g, struct player *p)
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
#define CBox(...) y += 10
#define Text(...) y += 10
#define FromUI
#include "p_settings.c"

   Lith_GUI_ScrollBegin(g, &CBIState(g)->settingscr, 15, 30, 280, 192, y);

   y = 0;

#define Label(label) \
   do { \
      static str const text = s"" label; \
      PrintText_str(text, s_cbifont, CR_WHITE, g->ox + 2,1, g->oy + y + 0,1); \
   } while(0)

#define Category(name) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 20)) \
      { \
         static str const text = s"" name; \
         PrintText_str(text, s_chfont, CR_LIGHTBLUE, g->ox + 140,0, g->oy + y + 5,1); \
      } \
      y += 20; \
   } while(0)

#define Bool(label, cvar) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(bool on = p->getCVarI(cvar);) \
      { \
         Label(label); \
         if(Lith_GUI_Button(g, on ? "On" : "Off", 280 - gui_p.btnlist.w, y, Pre(btnlist))) \
            p->setCVarI(cvar, !on); \
      } \
      y += 10; \
   } while(0)

#define Float(label, s, cvar, minima, maxima) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(k64 set = p->getCVarK(cvar), diff;) \
      { \
         Label(label); \
         if((diff = Lith_GUI_Slider(g, 280 - gui_p.slddef.w, y, minima, maxima, set, .suf = s))) \
            p->setCVarK(cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define Int(label, s, cvar, minima, maxima) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(i32 set = p->getCVarI(cvar), diff;) \
      { \
         Label(label); \
         if((diff = Lith_GUI_Slider(g, 280 - gui_p.slddef.w, y, minima, maxima, set, true, .suf = s))) \
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
         if(Lith_GUI_Button(g, on ? "On" : "Off", 280 - gui_p.btnlist.w, y, Pre(btnlist))) \
            ACS_SetCVar(cvar, !on); \
      } \
      y += 10; \
   } while(0)

#define ServerFloat(label, s, cvar, minima, maxima) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(k64 set = ACS_GetCVarFixed(cvar), diff;) \
      { \
         Label(label); \
         if((diff = Lith_GUI_Slider(g, 280 - gui_p.slddef.w, y, minima, maxima, set, .suf = s))) \
            ACS_SetCVarFixed(cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define ServerInt(label, s, cvar, minima, maxima) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(i32 set = ACS_GetCVar(cvar), diff;) \
      { \
         Label(label); \
         if((diff = Lith_GUI_Slider(g, 280 - gui_p.slddef.w, y, minima, maxima, set, true, .suf = s))) \
            ACS_SetCVar(cvar, set + diff); \
      } \
      y += 10; \
   } while(0)

#define Enum(label, cvar, minima, maxima, fmt, ...) \
   do { \
      if(!Lith_GUI_ScrollOcclude(g, &CBIState(g)->settingscr, y, 10)) \
         __with(i32 set = p->getCVarI(cvar);) \
      { \
         Label(label); \
         if(Lith_GUI_Button_Id(g, 0, .x = 280 - (gui_p.btnnexts.w*2), y, set == minima, Pre(btnprevs))) \
            p->setCVarI(cvar, set - 1); \
         if(Lith_GUI_Button_Id(g, 1, .x = 280 -  gui_p.btnnexts.w   , y, set == maxima, Pre(btnnexts))) \
            p->setCVarI(cvar, set + 1); \
         PrintTextFmt(fmt, __VA_ARGS__); \
         PrintText(s_cbifont, CR_WHITE, g->ox + 200,1, g->oy + y + 0,1); \
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
