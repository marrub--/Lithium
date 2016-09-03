#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"

// ---------------------------------------------------------------------------
// Shop Scripts.
//

//
// shopdef_t
//

typedef struct shopdef_s
{
   __str name;
   score_t cost;
   int count;
   __str class;
} shopdef_t;

static
shopdef_t shopdefs[] = {
// { "------------", ----------, ... },
   { "PlasmaAmmo",   14000     , 140, "Lith_PlasmaAmmo" },
   { "CannonAmmo",   30000     ,   6, "Lith_CannonAmmo" },
   { "DivSigil",     7772944   ,   1, "Lith_DivisionSigil" },
};

//
// Shop_CanBuy
//

bool Shop_CanBuy(player_t *p, shopdef_t *def)
{
   return ACS_CheckInventory(def->class) < ACS_GetMaxInventory(0, def->class) && p->score - def->cost >= 0;
}

//
// Shop_Buy
//

static
void Shop_Buy(player_t *p, shopdef_t *def)
{
   if(!Shop_CanBuy(p, def))
   {
      Log("Shop_Buy: YOU CANNOT BUY THESE THINGS");
      return;
   }
   
   ACS_GiveInventory(def->class, def->count);
   p->score -= def->cost;
}

//
// ---------------------------------------------------------------------------

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
   GUI_BEGIN_LIST(GUI_ID("uSCL"), gst, &hid, 20, 30, 152, &cbi->gst.scrlst[CBI_SCRLST_UPGRADES]);
   
   for(int i = 0; i < UPGR_MAX; i++)
   {
      GUI_LIST_OFFSETS(i, UPGR_MAX, 152, cbi->gst.scrlst[CBI_SCRLST_UPGRADES]);
      
      __str id = StrParam("uN%.2i", i);
      __str name = Language("LITH_TXT_UPGRADE_TITLE_%S", p->upgrades[i].info->name);
      
      if(GUI_Button(GUI_ID(id), gst, &hid, 0, addy, name, cbi->gst.lstst[CBI_LSTST_UPGRADES] == i, &gui_listbtnparm))
         cbi->gst.lstst[CBI_LSTST_UPGRADES] = i;
   }
   
   GUI_END_LIST(gst);
   
   int sel = cbi->gst.lstst[CBI_LSTST_UPGRADES];
   upgrade_t *upgr = &p->upgrades[sel];
   
   ACS_SetHudClipRect(95, 30, 200, 150, 200);
   
   __str lol = (sel == UPGR_lolsords) ? "folds" : "scr";
   __str cost = upgr->info->cost ? StrParam("%llu%S", upgr->info->cost, lol) : "---";
   
   HudMessageF("CBIFONT", "%S: %S\n\n%S", Language("LITH_COST"), cost, Language("LITH_TXT_UPGRADE_DESCR_%S", upgr->info->name));
   HudMessagePlain(hid--, 95.1, 30.1, TICSECOND);
   
   ACS_SetHudClipRect(0, 0, 0, 0);
   
   if(GUI_Button(GUI_ID("uBUY"), gst, &hid, 259, 170, "Buy", !Upgr_CanBuy(p, upgr)))
      Upgr_Buy(p, upgr);
   
   if(GUI_Button(GUI_ID("uACT"), gst, &hid, 209, 170, upgr->active ? "Deactivate" : "Activate", !upgr->owned))
      Upgr_ToggleActive(p, upgr);
   
   return hid;
}

//
// CBI_Tab_Shop
//

static
int CBI_Tab_Shop(player_t *p, int hid, cbi_t *cbi, gui_state_t *gst)
{
   int defcount = sizeof(shopdefs) / sizeof(*shopdefs);
   
   GUI_BEGIN_LIST(GUI_ID("sSCL"), gst, &hid, 20, 30, 152, &cbi->gst.scrlst[CBI_SCRLST_SHOP]);
   
   for(int i = 0; i < defcount; i++)
   {
      GUI_LIST_OFFSETS(i, defcount, 152, cbi->gst.scrlst[CBI_SCRLST_SHOP]);
      
      __str id = StrParam("sN%.2i", i);
      __str fullname = Language("LITH_TXT_SHOP_TITLE_%S", shopdefs[i].name);
      
      if(GUI_Button(GUI_ID(id), gst, &hid, 0, addy, fullname, cbi->gst.lstst[CBI_LSTST_SHOP] == i, &gui_listbtnparm))
         cbi->gst.lstst[CBI_LSTST_SHOP] = i;
   }
   
   GUI_EndOffset(gst);
   GUI_END_LIST(gst);
   
   shopdef_t *def = &shopdefs[cbi->gst.lstst[CBI_LSTST_SHOP]];
   
   __str cost = def->cost ? StrParam("%lluscr", def->cost) : "---";
   
   HudMessageF("CBIFONT", "%S: %S\n\n%S", Language("LITH_COST"), cost, Language("LITH_TXT_SHOP_DESCR_%S", def->name));
   HudMessagePlain(hid--, 95.1, 30.1, TICSECOND);
   
   if(GUI_Button(GUI_ID("sBUY"), gst, &hid, 259, 170, "Buy", !Shop_CanBuy(p, def)))
      Shop_Buy(p, def);
   
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
      
      static __str tabnames[5] = { "Upgrades", "Shop", "BIP ", "Statistics", "Settings" };
      for(int i = 0; i < 5; i++)
         if(GUI_Button(GUI_ID(tabnames[i]), gst, &hid, 13 + (GUI_TAB_W * i), 13, tabnames[i], cbi->gst.tabst[CBI_TABST_MAIN] == i, &gui_tabparm))
            cbi->gst.tabst[CBI_TABST_MAIN] = i;
      
      switch(cbi->gst.tabst[CBI_TABST_MAIN])
      {
      case TAB_UPGRADES: hid = CBI_Tab_Upgrades(p, hid, cbi, gst); break;
      case TAB_SHOP: hid = CBI_Tab_Shop(p, hid, cbi, gst); break;
      case TAB_STATISTICS: hid = CBI_Tab_Statistics(p, hid, cbi, gst); break;
      case TAB_BIP:
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

