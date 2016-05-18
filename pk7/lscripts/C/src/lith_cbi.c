#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"

// ---------------------------------------------------------------------------
// Key Scripts.
//

//
// Lith_KeyOpenCBI
//
// Called when the CBI is toggled.
//

[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyOpenCBI(void)
{
   player_t *p = &players[ACS_PlayerNumber()];
   
   if(p->dead)
      return;
   
   p->cbi.open = !p->cbi.open;
   
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

//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Computer-Brain Interface (CBI) Scripts.
//

//
// CBI_Tab_Upgrades
//

static
int CBI_Tab_Upgrades(player_t *p, int hid, cbi_t *cbi, gui_state_t *gst)
{
   GUI_BEGIN_LIST(GUI_ID("uSCL"), gst, &hid, 20, 30, 152, &cbi->gst.scrlst[0]);
   
   for(int i = 0; i < UPGR_MAX; i++)
   {
      GUI_LIST_OFFSETS(i, UPGR_MAX, 152, cbi->gst.scrlst[0]);
      
      __str id = StrParam("uN%.2i", i);
      if(GUI_Button(GUI_ID(id), gst, &hid, 0, addy, upgrade_names[i], cbi->gst.lstst[0] == i, &gui_listbtnparm))
         cbi->gst.lstst[0] = i;
   }
   
   GUI_END_LIST(gst);
   
   upgrade_t *upgr = &p->upgrades[cbi->gst.lstst[0]];
   
   ACS_SetHudClipRect(95, 30, 200, 150, 200);
   
   HudMessageF("CBIFONT", "%S", upgr->description);
   HudMessagePlain(hid--, 95.1, 30.1, TICSECOND);
   
   ACS_SetHudClipRect(0, 0, 0, 0);
   
   if(GUI_Button(GUI_ID("uBUY"), gst, &hid, 259, 170, "Buy", !Upgr_CanBuy(p, upgr)))
      Upgr_Buy(p, &p->upgrades[cbi->gst.lstst[0]]);
   
   if(GUI_Button(GUI_ID("uACT"), gst, &hid, 209, 170, upgr->active ? "Deactivate" : "Activate", !upgr->owned))
      Upgr_ToggleActive(p, &p->upgrades[cbi->gst.lstst[0]]);
   
   return hid;
}

//
// CBI_Tab_Statistics
//

static
int CBI_Tab_Statistics(player_t *p, int hid, cbi_t *cbi, gui_state_t *gst)
{
   HudMessageF("SMALLFNT", "\Cj%S", p->name); HudMessagePlain(hid--, 20.1, 0.1 + 30, TICSECOND);
   HudMessageF("CBIFONT", "Secrets Found: %i", p->secretsfound); HudMessagePlain(hid--, 23.1, 0.1 + 40 + (8 * 0), TICSECOND);
   HudMessageF("CBIFONT", "Weapons Held: %i", p->weaponsheld); HudMessagePlain(hid--, 23.1, 0.1 + 40 + (8 * 1), TICSECOND);
   HudMessageF("CBIFONT", "Health Used: %li", p->healthused); HudMessagePlain(hid--, 23.1, 0.1 + 40 + (8 * 2), TICSECOND);
   HudMessageF("CBIFONT", "Score Used: %lli", p->scoreused); HudMessagePlain(hid--, 23.1, 0.1 + 40 + (8 * 3), TICSECOND);
   HudMessageF("CBIFONT", "Armor Used: %li", p->armorused); HudMessagePlain(hid--, 23.1, 0.1 + 40 + (8 * 4), TICSECOND);
   HudMessageF("CBIFONT", "Health Sum: %li", p->healthsum); HudMessagePlain(hid--, 23.1, 0.1 + 40 + (8 * 5), TICSECOND);
   HudMessageF("CBIFONT", "Score Sum: %lli", p->scoresum); HudMessagePlain(hid--, 23.1, 0.1 + 40 + (8 * 6), TICSECOND);
   HudMessageF("CBIFONT", "Armor Sum: %li", p->armorsum); HudMessagePlain(hid--, 23.1, 0.1 + 40 + (8 * 7), TICSECOND);
   return hid;
}

//
// Lith_PlayerUpdateCBI
//

[[__call("ScriptI")]]
void Lith_PlayerUpdateCBI(player_t *p)
{
   if(p->cbi.open)
   {
      cbi_t *cbi = &p->cbi;
      
      gui_state_t *gst = &cbi->gst.ggst;
      
      //
      // Cursor position state.
      gst->cur.x -= p->yawv * 800.0f;
      
      if(ACS_GetCVar("invertmouse"))
         gst->cur.y += p->pitchv * 800.0f;
      else
         gst->cur.y -= p->pitchv * 800.0f;
      
      gst->cur.x = minmax(gst->cur.x, 0, 320);
      gst->cur.y = minmax(gst->cur.y, 0, 200);
      
      //
      // Click state.
      gst->cur.click = GUI_CLICK_NONE;
      
      if(p->buttons & BT_ATTACK)
         gst->cur.click |= GUI_CLICK_LEFT;
      
      if(p->buttons & BT_ALTATTACK)
         gst->cur.click |= GUI_CLICK_RIGHT;
      
      //
      // GUI.
      int hid = hid_end_cbi;
      
      GUI_Begin(gst);
      
      DrawSpriteAlpha("lgfx/UI/Background.png", hid--, 0.1, 0.1, TICSECOND, 0.7);
      
      if(GUI_Button(GUI_ID("Exit"), gst, &hid, 296, 13, null, false, &gui_exitparm))
         Lith_KeyOpenCBI();
      
      static __str tabnames[5] = { "Upgrades", "BIP ", "Shop", "Statistics", "Settings" };
      for(int i = 0; i < 5; i++)
         if(GUI_Button(GUI_ID(tabnames[i]), gst, &hid, 13 + (GUI_TAB_W * i), 13, tabnames[i], cbi->gst.tabst[0] == i, &gui_tabparm))
            cbi->gst.tabst[0] = i;
      
      switch(cbi->gst.tabst[0])
      {
      case TAB_UPGRADES: hid = CBI_Tab_Upgrades(p, hid, cbi, gst); break;
      case TAB_STATISTICS: hid = CBI_Tab_Statistics(p, hid, cbi, gst); break;
      case TAB_BIP:
      case TAB_SHOP:
      case TAB_SETTINGS:
         HudMessageRainbowsF("BIGFONT", "404 Page Not Found");
         HudMessagePlain(hid--, 40.1, 40.1, TICSECOND);
         break;
      }
      
      DrawSpritePlain("lgfx/UI/Cursor.png", hid--, 0.1 + (int)gst->cur.x, 0.1 + (int)gst->cur.y, TICSECOND);
      
      GUI_End(gst);
   }
}

//
// ---------------------------------------------------------------------------

