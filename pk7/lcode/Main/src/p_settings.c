#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

#include <ctype.h>


//----------------------------------------------------------------------------
// Static Functions
//

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
   case cbi_theme_green:  return "Green";
   case cbi_theme_rose:   return "Rose";
   case cbi_theme_umi:    return "Umi";
   case cbi_theme_ender:  return "Ender";
   case cbi_theme_orange: return "Orange";
   case cbi_theme_grey:   return "Grey";
   default:               return "Unknown";
   }
}

//
// ToggleAutoBuy
//
static void ToggleAutoBuy(player_t *p, int num)
{
   p->autobuy[num] = !p->autobuy[num];
   p->saveData();
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_CBITab_Settings
//
void Lith_CBITab_Settings(gui_state_t *g, player_t *p)
{
   int y = 0;
   
#define Category(...) y += 20;
#define Bool(...) y += 10;
#define ServerBool(...) y += 10;
#define ServerFloat(...) y += 10;
#define Float(...) y += 10;
#define Int(...) y += 10;
#define Enum(...) y += 10;
#define CBox(...) y += 10;
#include "lith_settings.h"
   
   Lith_GUI_ScrollBegin(g, st_settingscr, 15, 30, 280, 192, y);
   
   y = 0;
   
#define Label(label) \
   HudMessageF("CBIFONT", label); \
   HudMessagePlain(g->hid--, g->ox + 2.1, g->oy + y + 0.1, TICSECOND)
   
#define Category(name) \
   if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 20)) \
   { \
   HudMessageF("CNFONT", "\Cn" name); \
   HudMessagePlain(g->hid--, g->ox + 140.0, g->oy + y + 5.1, TICSECOND); \
   } \
   y += 20;

#define Bool(label, cvar) \
   if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
   { \
   __with(bool on = Lith_GetPCVarInt(p, cvar);) \
   { \
      Label(label); \
      if(Lith_GUI_Button(g, on ? "On" : "Off", 280 - btnlist.w, y, .preset = &btnlist)) \
         Lith_SetPCVarInt(p, cvar, !on); \
   } \
   } \
   y += 10;

#define ServerBool(label, cvar) \
   if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
   { \
   __with(bool on = ACS_GetCVar(cvar);) \
   { \
      Label(label); \
      if(Lith_GUI_Button(g, on ? "On" : "Off", 280 - btnlist.w, y, .preset = &btnlist)) \
         ACS_SetCVar(cvar, !on); \
   } \
   } \
   y += 10;

#define ServerFloat(label, cvar, minima, maxima) \
   if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
   { \
   __with(double set = ACS_GetCVarFixed(cvar), diff;) \
   { \
      Label(label); \
      if((diff = Lith_GUI_Slider(g, 280 - slddefault.w, y, minima, maxima, set))) \
         ACS_SetCVarFixed(cvar, set + diff); \
   } \
   } \
   y += 10;

#define Float(label, cvar, minima, maxima) \
   if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
   { \
   __with(double set = Lith_GetPCVarFixed(p, cvar), diff;) \
   { \
      Label(label); \
      if((diff = Lith_GUI_Slider(g, 280 - slddefault.w, y, minima, maxima, set))) \
         Lith_SetPCVarFixed(p, cvar, set + diff); \
   } \
   } \
   y += 10;

#define Int(label, cvar, minima, maxima) \
   if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
   { \
   __with(int set = Lith_GetPCVarInt(p, cvar), diff;) \
   { \
      Label(label); \
      if((diff = Lith_GUI_Slider(g, 280 - slddefault.w, y, minima, maxima, set, true))) \
         Lith_SetPCVarInt(p, cvar, set + diff); \
   } \
   } \
   y += 10;

#define Enum(label, cvar, minima, maxima, fmt, ...) \
   if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
   { \
   __with(int set = Lith_GetPCVarInt(p, cvar);) \
   { \
      Label(label); \
      if(Lith_GUI_Button_Id(g, 0, .x = 280 - (btnnexts.w*2), y, set == minima, .preset = &btnprevs)) \
         Lith_SetPCVarInt(p, cvar, set - 1); \
      if(Lith_GUI_Button_Id(g, 1, .x = 280 -  btnnexts.w   , y, set == maxima, .preset = &btnnexts)) \
         Lith_SetPCVarInt(p, cvar, set + 1); \
      HudMessageF("CBIFONT", fmt, __VA_ARGS__); \
      HudMessagePlain(g->hid--, g->ox + 200.1, g->oy + y + 0.1, TICSECOND); \
   } \
   } \
   y += 10;

#define CBox(label, on, ...) \
   if(!Lith_GUI_ScrollOcclude(g, st_settingscr, y, 10)) \
   { \
   Label(label); \
   if(Lith_GUI_Checkbox(g, on, 240, y + 5, .preset = &cbxsmall)) \
      __VA_ARGS__; \
   } \
   y += 10;

#include "lith_settings.h"
#undef Label
   
   Lith_GUI_ScrollEnd(g, st_settingscr);
}

// EOF

