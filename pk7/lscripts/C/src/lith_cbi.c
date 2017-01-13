#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"


//----------------------------------------------------------------------------
// Static Functions
//

//
// CBI_Tab_Statistics
//
static int CBI_Tab_Statistics(player_t *p, int hid, cbi_t *cbi, gui_state_t *gst)
{
#define Stat(n, name, f, x) \
   HudMessageF("CBIFONT", name); HudMessagePlain(hid--, 23.1,  0.1 + 70 + (8 * n), TICSECOND); \
   HudMessageF("CBIFONT", f, x); HudMessagePlain(hid--, 300.2, 0.1 + 70 + (8 * n), TICSECOND)
   
   HudMessageF("SMALLFNT", "\Cj%S", p->name); HudMessagePlain(hid--, 20.1, 60.1, TICSECOND);
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
   return hid;
}


//----------------------------------------------------------------------------
// External Functions
//

//
// Lith_PlayerUpdateCBI
//
[[__call("ScriptS")]]
void Lith_PlayerUpdateCBI(player_t *p)
{
   if(p->cbi.open)
   {
      cbi_t       *cbi = &p->cbi;
      gui_state_t *gst = &cbi->gst.ggst;
      
      //
      // Cursor position state
      gst->cur.x -= p->yawv * 800.0f;
      
      bool inverted = ACS_GetUserCVar(p->number, "lith_player_invertmouse");
      
      if(ACS_GameType() == GAME_SINGLE_PLAYER)
         inverted = inverted || ACS_GetCVar("invertmouse");
      
      if(inverted)
         gst->cur.y += p->pitchv * 800.0f;
      else
         gst->cur.y -= p->pitchv * 800.0f;
      
      gst->cur.x = minmax(gst->cur.x, 0, 320);
      gst->cur.y = minmax(gst->cur.y, 0, 200);
      
      //
      // Click state
      gst->cur.click = GUI_CLICK_NONE;
      
      if(p->buttons & BT_ATTACK)
         gst->cur.click |= GUI_CLICK_LEFT;
      
      if(p->buttons & BT_ALTATTACK)
         gst->cur.click |= GUI_CLICK_RIGHT;
      
      //
      // GUI
      int hid = hid_end_cbi;
      
      GUI_Begin(gst);
      
      DrawSpriteAlpha("lgfx/UI/Background.png", hid--, 0.1, 0.1, TICSECOND, 0.7);
      
      if(GUI_Button(GUI_ID("Exit"), gst, &hid, 296, 13, null, false, &gui_exitparm))
         Lith_KeyOpenCBI();
      
      static __str tabnames[5] = {"Upgrades", "Shop", "Info", "Statistics", "Settings"};
      for(int i = 0; i < 5; i++)
         if(GUI_Button(GUI_ID(tabnames[i]), gst, &hid, 13 + (GUI_TAB_W * i), 13, tabnames[i], cbi->gst.tabst[CBI_TABST_MAIN] == i, &gui_tabparm))
            cbi->gst.tabst[CBI_TABST_MAIN] = i;
      
      switch(cbi->gst.tabst[CBI_TABST_MAIN])
      {
      case TAB_UPGRADES:   hid = CBI_Tab_Upgrades(p, hid, cbi, gst);   break;
      case TAB_SHOP:       hid = CBI_Tab_Shop(p, hid, cbi, gst);       break;
      case TAB_STATISTICS: hid = CBI_Tab_Statistics(p, hid, cbi, gst); break;
      case TAB_BIP:        hid = CBI_Tab_BIP(p, hid, cbi, gst);        break;
      case TAB_SETTINGS:
         HudMessageRainbowsF("BIGFONT", "404 Page Not Found");
         HudMessagePlain(hid--, 40.1, 40.1, TICSECOND);
         break;
      }
      
      DrawSpritePlain("lgfx/UI/Cursor.png", hid--, 0.1 + (int)gst->cur.x, 0.1 + (int)gst->cur.y, TICSECOND);
      
      GUI_End(gst);
   }
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

