#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_world.h"


//----------------------------------------------------------------------------
// Static Functions
//

//
// Lith_CBITab_Log
//
static void Lith_CBITab_Log(gui_state_t *g, player_t *p)
{
   size_t num = 0;
   int i = 0;
   
   logmap_t *selmap;
   list_t *sel = g->st[st_logsel].vp;
   
   if((sel = g->st[st_logsel].vp) == null)
      sel = p->loginfo.maps.next;
   
   if(Lith_GUI_Button(g, .x = 25, 28, .preset = &btnprev))
      if((sel = sel->prev) == &p->loginfo.maps)
         sel = sel->prev;
   
   if(Lith_GUI_Button(g, .x = 25 + btnprev.w, 28, .preset = &btnnext))
      if((sel = sel->next) == &p->loginfo.maps)
         sel = sel->next;
   
   g->st[st_logsel].vp = sel;
   selmap = sel->object;
   
   HudMessageF("CBIFONT", "%S", selmap->name);
   HudMessagePlain(g->hid--, 28.1 + btnprev.w + btnnext.w, 30.1, TICSECOND);
   
   Lith_ForList(logdata_t *logdata, p->loginfo.full)
      num += (logdata->from == selmap->levelnum);
   
   Lith_GUI_ScrollBegin(g, st_logscr, 15, 40, 280, 148, num * 8);
   
   Lith_ForList(logdata_t *logdata, p->loginfo.full)
   {
      if(logdata->from != selmap->levelnum)
         continue;
      
      int y = 8 * i++;
      
      if(Lith_GUI_ScrollOcclude(g, st_logscr, y, 8))
         continue;
      
      DrawSpritePlain("lgfx/UI/LogList.png", g->hid--, g->ox + 0.1, y + g->oy + 0.1, TICSECOND);
      
      HudMessageF("CBIFONT", "%S", logdata->info);
      HudMessageParams(0, g->hid--, CR_GREEN, g->ox + 2.1, y + g->oy + 1.1, TICSECOND);
   }
   
   
   Lith_GUI_ScrollEnd(g, st_logscr);
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
#define Float(...) y += 10;
#include "lith_settings.h"
   
   Lith_GUI_ScrollBegin(g, st_settingscr, 15, 30, 280, 152, y);
   
   y = 0;
   
#define Label(label) \
   HudMessageF("CBIFONT", label); \
   HudMessagePlain(g->hid--, g->ox + 2.1, g->oy + y + 0.1, TICSECOND)
   
#define Category(name) \
   HudMessageF("CNFONT", "\Cn" name); \
   HudMessagePlain(g->hid--, g->ox + 140.0, g->oy + y + 5.1, TICSECOND); \
   y += 20;

#define Bool(label, cvar) \
   __with(bool on = ACS_GetUserCVar(p->number, cvar);) \
   { \
      Label(label); \
      if(Lith_GUI_Button(g, on ? "On" : "Off", 280 - btnlist.w, y, .preset = &btnlist)) \
         ACS_SetUserCVar(p->number, cvar, !on); \
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

#define Float(label, cvar, minima, maxima) \
   __with(float set = ACS_GetUserCVarFixed(p->number, cvar), diff;) \
   { \
      Label(label); \
      if((diff = Lith_GUI_Slider(g, 280 - slddefault.w, y, minima, maxima, set))) \
         ACS_SetUserCVarFixed(p->number, cvar, set + diff); \
      y += 10; \
   }

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
   Stat(7,  "Secrets Found",     "%i",   secretsfound);
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

void Lith_CBITab_Upgrades(gui_state_t *g, player_t *p);
void Lith_CBITab_Shop(gui_state_t *g, player_t *p);
void Lith_CBITab_BIP(gui_state_t *g, player_t *p);

//
// Lith_PlayerUpdateCBI
//
[[__call("ScriptS")]]
void Lith_PlayerUpdateCBI(player_t *p)
{
   if(p->cbi.open)
   {
      gui_state_t *g = &p->cbi.guistate;
      
      Lith_GUI_UpdateState(g, p);
      Lith_GUI_Begin(g);
      
      DrawSpriteAlpha("lgfx/UI/Background.png", g->hid--, 0.1, 0.1, TICSECOND, 0.7);
      
      if(Lith_GUI_Button(g, .x = 296, 13, .preset = &btnexit))
         Lith_KeyOpenCBI();
      
      static __str tabnames[cbi_tab_max] = {"Upgrades", "Shop", "Info", "Statistics", "Settings", "Log"};
      for(int i = 0; i < cbi_tab_max; i++)
         if(Lith_GUI_Button_Id(g, i, tabnames[i], btntab.w * i + 13, 13, i == g->st[st_maintab].i, .preset = &btntab))
            g->st[st_maintab].i = i;
      
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
}

//
// Lith_PlayerResetCBI
//
void Lith_PlayerResetCBI(player_t *p)
{
   p->cbi.open = false;
   p->cbi.guistate.cx = 320 / 2;
   p->cbi.guistate.cy = 200 / 2;
   
   Lith_GUI_Init(&p->cbi.guistate, st_max);
}


//----------------------------------------------------------------------------
// Scripts
//

//
// Lith_KeyOpenCBI
//
// Called when the CBI is toggled.
//
[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyOpenCBI(void)
{
   player_t *p = LocalPlayer;
   
   if(p->dead)
      return;
   
   p->cbi.open = !p->cbi.open;
   p->bip.curcategory = BIPC_MAIN;
   p->bip.curpage     = null;
   p->bip.curpagenum  = -1;
   
   if(p->cbi.open)
   {
      p->frozen++;
      ACS_LocalAmbientSound("player/cbi/open", 127);
   }
   else
   {
      p->frozen--;
      ACS_LocalAmbientSound("player/cbi/close", 127);
   }
}

// EOF

