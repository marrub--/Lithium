#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_world.h"

#include <ctype.h>


//----------------------------------------------------------------------------
// Static Functions
//

//
// ColorName
//
__str ColorName(char ch)
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
// ToggleAutoBuy
//
void ToggleAutoBuy(player_t *p, int num)
{
   p->autobuy[num] = !p->autobuy[num];
   p->saveData();
}

//
// Lith_CBITab_Settings
//
static void Lith_CBITab_Settings(gui_state_t *g, player_t *p)
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
   HudMessageF("CNFONT", "\Cn" name); \
   HudMessagePlain(g->hid--, g->ox + 140.0, g->oy + y + 5.1, TICSECOND); \
   y += 20;

#define Bool(label, cvar) \
   __with(bool on = Lith_GetPCVarInt(p, cvar);) \
   { \
      Label(label); \
      if(Lith_GUI_Button(g, on ? "On" : "Off", 280 - btnlist.w, y, .preset = &btnlist)) \
         Lith_SetPCVarInt(p, cvar, !on); \
      y += 10; \
   }

#define ServerBool(label, cvar) \
   __with(bool on = ACS_GetCVar(cvar);) \
   { \
      Label(label); \
      if(Lith_GUI_Button(g, on ? "On" : "Off", 280 - btnlist.w, y, .preset = &btnlist)) \
         ACS_SetCVar(cvar, !on); \
      y += 10; \
   }

#define ServerFloat(label, cvar, minima, maxima) \
   __with(double set = ACS_GetCVarFixed(cvar), diff;) \
   { \
      Label(label); \
      if((diff = Lith_GUI_Slider(g, 280 - slddefault.w, y, minima, maxima, set))) \
         ACS_SetCVarFixed(cvar, set + diff); \
      y += 10; \
   }

#define Float(label, cvar, minima, maxima) \
   __with(double set = Lith_GetPCVarFixed(p, cvar), diff;) \
   { \
      Label(label); \
      if((diff = Lith_GUI_Slider(g, 280 - slddefault.w, y, minima, maxima, set))) \
         Lith_SetPCVarFixed(p, cvar, set + diff); \
      y += 10; \
   }

#define Int(label, cvar, minima, maxima) \
   __with(int set = Lith_GetPCVarInt(p, cvar), diff;) \
   { \
      Label(label); \
      if((diff = Lith_GUI_Slider(g, 280 - slddefault.w, y, minima, maxima, set, true))) \
         Lith_SetPCVarInt(p, cvar, set + diff); \
      y += 10; \
   }

#define Enum(label, cvar, minima, maxima, fmt, ...) \
   __with(int set = Lith_GetPCVarInt(p, cvar);) \
   { \
      Label(label); \
      if(Lith_GUI_Button_Id(g, 0, .x = 280 - (btnnexts.w*2), y, set == minima, .preset = &btnprevs)) \
         Lith_SetPCVarInt(p, cvar, set - 1); \
      if(Lith_GUI_Button_Id(g, 1, .x = 280 -  btnnexts.w   , y, set == maxima, .preset = &btnnexts)) \
         Lith_SetPCVarInt(p, cvar, set + 1); \
      HudMessageF("CBIFONT", fmt, __VA_ARGS__); \
      HudMessagePlain(g->hid--, g->ox + 200.1, g->oy + y + 0.1, TICSECOND); \
      y += 10; \
   }

#define CBox(label, on, ...) \
   Label(label); \
   if(Lith_GUI_Checkbox(g, on, 240, y + 5, .preset = &cbxsmall)) \
      __VA_ARGS__; \
   y += 10;

#include "lith_settings.h"
#undef Label
   
   Lith_GUI_ScrollEnd(g, st_settingscr);
}

//
// Lith_CBITab_Statistics
//
static void Lith_CBITab_Statistics(gui_state_t *g, player_t *p)
{
#define Stat(n, name, f, x) \
   HudMessageF("CBIFONT", name); HudMessagePlain(g->hid--, 23.1,  0.1 + 70 + (8 * n), TICSECOND); \
   HudMessageF("CBIFONT", f, x); HudMessagePlain(g->hid--, 300.2, 0.1 + 70 + (8 * n), TICSECOND)
   
   HudMessageF("SMALLFNT", "\Cj%S", p->name); HudMessagePlain(g->hid--, 20.1, 60.1, TICSECOND);
   Stat(0,  "Weapons Found",     "%i",   p->weaponsheld);
   Stat(1,  "Health Used",       "%li",  p->healthused);
   Stat(2,  "Health Sum",        "%li",  p->healthsum);
   Stat(3,  "Score Used",        "%lli", p->scoreused);
   Stat(4,  "Score Sum",         "%lli", p->scoresum);
   Stat(5,  "Armor Used",        "%li",  p->armorused);
   Stat(6,  "Armor Sum",         "%li",  p->armorsum);
   Stat(7,  "Secrets Found",     "%i",   world.secretsfound);
   Stat(8,  "Units Travelled",   "%i",   p->unitstravelled);
// Stat(9,  "Enemies Defeated",  "%i",   0);
// Stat(10, "Bosses Defeated",   "%i",   0);
   Stat(9,  "Upgrades Owned",    "%i",   p->upgradesowned);
   Stat(10, "Items Bought",      "%i",   p->itemsbought);
// Stat(13, "Rituals Performed", "%i",   0);
   
#undef Stat
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_PlayerUpdateCBI
//
[[__call("ScriptS")]]
void Lith_PlayerUpdateCBI(player_t *p)
{
   gui_state_t *g = &p->cbi.guistate;
   
   Lith_GUI_Begin(g, hid_end_cbi, 320, 240);
   Lith_GUI_UpdateState(g, p);
   
   DrawSpriteAlpha("lgfx/UI/Background.png", g->hid--, 0.1, 0.1, TICSECOND, 0.7);
   
   if(Lith_GUI_Button(g, .x = 296, 13, .preset = &btnexit))
      p->useGUI(GUI_CBI);
   
   static __str tabnames[cbi_tab_max] = {"Upgrades", "Shop", "Info", "Statistics", "Settings", "Log"};
   for(int i = 0; i < cbi_tab_max; i++)
      if(Lith_GUI_Button_Id(g, i, tabnames[i], btntab.w * i + 13, 13, i == g->st[st_maintab].i, .preset = &btntab))
         g->st[st_maintab].i = i;
   
   extern void Lith_CBITab_Upgrades(gui_state_t *g, player_t *p);
   extern void Lith_CBITab_Shop    (gui_state_t *g, player_t *p);
   extern void Lith_CBITab_BIP     (gui_state_t *g, player_t *p);
   extern void Lith_CBITab_Log     (gui_state_t *g, player_t *p);
   
   switch(g->st[st_maintab].i)
   {
   case cbi_tab_upgrades:   Lith_CBITab_Upgrades  (g, p); break;
   case cbi_tab_shop:       Lith_CBITab_Shop      (g, p); break;
   case cbi_tab_bip:        Lith_CBITab_BIP       (g, p); break;
   case cbi_tab_statistics: Lith_CBITab_Statistics(g, p); break;
   case cbi_tab_settings:   Lith_CBITab_Settings  (g, p); break;
   case cbi_tab_log:        Lith_CBITab_Log       (g, p); break;
   }
   
   Lith_GUI_End(g);
}

//
// Lith_PlayerResetCBI
//
void Lith_PlayerResetCBI(player_t *p)
{
   p->cbi.guistate.cx = 320 / 2;
   p->cbi.guistate.cy = 200 / 2;
   
   Lith_GUI_Init(&p->cbi.guistate, st_max);
   
   p->cbi.guistate.st[st_upgrselold].i = -1;
}


//----------------------------------------------------------------------------
// Scripts
//

//
// Lith_KeyOpenCBI
//
[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyOpenCBI(void)
{
   if(ACS_Timer() < 10) return;
   
   player_t *p = LocalPlayer;
   
   if(p->dead)
      return;
   
   p->bip.curcategory = BIPC_MAIN;
   p->bip.curpage     = null;
   p->bip.curpagenum  = -1;
   
   p->useGUI(GUI_CBI);
}

// EOF

