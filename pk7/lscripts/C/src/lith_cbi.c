#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"


//----------------------------------------------------------------------------
// Static Functions
//

//
// Lith_CBITab_Settings
//
static void Lith_CBITab_Settings(gui_state_t *g, player_t *p)
{
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
   Stat(0,  "Weapons Held",      "%i",   p->weaponsheld);
   Stat(1,  "Health Used",       "%li",  p->healthused);
   Stat(2,  "Health Sum",        "%li",  p->healthsum);
   Stat(3,  "Score Used",        "%lli", p->scoreused);
   Stat(4,  "Score Sum",         "%lli", p->scoresum);
   Stat(5,  "Armor Used",        "%li",  p->armorused);
   Stat(6,  "Armor Sum",         "%li",  p->armorsum);
   Stat(7,  "Secrets Found",     "%i",   p->secretsfound);
   Stat(8,  "Units Travelled",   "%llu", p->unitstravelled);
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
      
      static __str tabnames[cbi_tab_max] = {"Upgrades", "Shop", "Info", "Statistics", "Settings"};
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
   player_t *p = Lith_LocalPlayer;
   
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

