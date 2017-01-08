#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"


//----------------------------------------------------------------------------
// Static Objects
//

typedef struct shopdef_s
{
   __str name;
   score_t cost;
   int count;
   __str class;
} shopdef_t;

static shopdef_t shopdefs[] = {
// {"Name--------", Cost------, Cnt, Class---------------},
   {"PlasmaAmmo",   14000     , 400, "Lith_PlasmaAmmo"   },
   {"CannonAmmo",   30000     ,   6, "Lith_CannonAmmo"   },
// {"DivSigil",     7772944   ,   1, "Lith_DivisionSigil"},
};

static size_t const shopdefsnum = sizeof(shopdefs) / sizeof(*shopdefs);


//----------------------------------------------------------------------------
// Static Functions
//

//
// Shop_CanBuy
//
bool Shop_CanBuy(player_t *p, shopdef_t *def)
{
   int cur = ACS_CheckInventory(def->class);
   int max = ACS_GetMaxInventory(0, def->class);
   return cur < max && p->score - def->cost >= 0;
}

//
// Shop_Buy
//
static void Shop_Buy(player_t *p, shopdef_t *def)
{
   if(!Shop_CanBuy(p, def))
   {
      Log("Shop_Buy: YOU CANNOT BUY THESE THINGS");
      return;
   }
   
   ACS_GiveInventory(def->class, def->count);
   Lith_TakeScore(p, def->cost);
   p->itemsbought++;
}

//
// CBI_Tab_Upgrades
//
static int CBI_Tab_Upgrades(player_t *p, int hid, cbi_t *cbi, gui_state_t *gst)
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
   
   ACS_SetHudClipRect(111, 30, 184, 150, 184);
   
   __str mark;
   switch(sel)
   {
   case UPGR_lolsords:   mark = "folds"; break;
   case UPGR_TorgueMode: mark = "$";     break;
   default:              mark = "scr";   break;
   }
   
   __str cost = upgr->info->cost ? StrParam("%llu%S", upgr->info->cost, mark) : "---";
   
   HudMessageF("CBIFONT", "%S: %S\n\n%S", Language("LITH_COST"), cost, Language("LITH_TXT_UPGRADE_DESCR_%S", upgr->info->name));
   HudMessagePlain(hid--, 111.1, 30.1, TICSECOND);
   
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
static int CBI_Tab_Shop(player_t *p, int hid, cbi_t *cbi, gui_state_t *gst)
{
   GUI_BEGIN_LIST(GUI_ID("sSCL"), gst, &hid, 20, 30, 152, &cbi->gst.scrlst[CBI_SCRLST_SHOP]);
   
   for(int i = 0; i < shopdefsnum; i++)
   {
      GUI_LIST_OFFSETS(i, shopdefsnum, 152, cbi->gst.scrlst[CBI_SCRLST_SHOP]);
      
      __str id = StrParam("sN%.2i", i);
      __str fullname = Language("LITH_TXT_SHOP_TITLE_%S", shopdefs[i].name);
      
      if(GUI_Button(GUI_ID(id), gst, &hid, 0, addy, fullname, cbi->gst.lstst[CBI_LSTST_SHOP] == i, &gui_listbtnparm))
         cbi->gst.lstst[CBI_LSTST_SHOP] = i;
   }
   
   GUI_END_LIST(gst);
   
   shopdef_t *def = &shopdefs[cbi->gst.lstst[CBI_LSTST_SHOP]];
   
   __str cost = def->cost ? StrParam("%lluscr", def->cost) : "---";
   
   HudMessageF("CBIFONT", "%S: %S\n\n%S", Language("LITH_COST"), cost, Language("LITH_TXT_SHOP_DESCR_%S", def->name));
   HudMessagePlain(hid--, 111.1, 30.1, TICSECOND);
   
   if(GUI_Button(GUI_ID("sBUY"), gst, &hid, 259, 170, "Buy", !Shop_CanBuy(p, def)))
      Shop_Buy(p, def);
   
   return hid;
}

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

//
// CBI_Tab_BIP
//
static int CBI_Tab_BIP(player_t *p, int hid, cbi_t *cbi, gui_state_t *gst)
{
   static gui_button_parm_t const btnp = {
      .f_gfx_def = "",
      .f_gfx_hot = "",
      .dim_x = 180,
      .dim_y = 9
   };
   
   static gui_button_parm_t const backbtnp = {
      .f_gfx_def = "",
      .f_gfx_hot = "",
      .dim_y = 9
   };
   
   bip_t *bip = &p->bip;
   int avail, max;
   
   if(bip->curcategory == BIPC_MAIN)
   {
      HudMessageF("CBIFONT", "\CTINFO");
      HudMessagePlain(hid--, 160.4, 70.1, TICSECOND);
      
#define LITH_X(n, id, name, capt) \
      if(GUI_Button(GUI_ID("b" #id), gst, &hid, 70, 80 + (n * 10), capt, false, &btnp)) \
      { \
         bip->curcategory = BIPC_##name; \
         bip->curpagenum  = -1; \
         bip->curpage     = null; \
      }
#include "lith_bip.h"
      
      avail = bip->pageavail;
      max   = bip->pagemax;
   }
   else
   {
      GUI_BEGIN_LIST(GUI_ID("bSCL"), gst, &hid, 20, 50, 130, &bip->scroll);
      
      dlist_t *l = bip->infogr[bip->curcategory];
      size_t n = DList_GetLength(l);
      size_t i = 0;
      for(slist_t *rover = l->head; rover; rover = rover->next, i++)
      {
         bippage_t *page = rover->data.vp;
         GUI_LIST_OFFSETS(i, n, 130, cbi->gst.scrlst[CBI_SCRLST_UPGRADES]);
         
         __str id   = StrParam("bN%.2i", i);
         __str name = StrParam("%S%S", bip->curpagenum == i ? "\Ci" : "", Language("LITH_TXT_INFO_SHORT_%S", page->name));
         
         if(GUI_Button(GUI_ID(id), gst, &hid, 0, addy, name, !page->unlocked || bip->curpagenum == i, &gui_listbtnparm))
         {
            bip->curpagenum = i;
            bip->curpage    = page;
         }
      }
      
      GUI_END_LIST(gst);
      
      if(bip->curpage)
      {
         ACS_SetHudClipRect(111, 40, 184, 140, 184);
         
         HudMessageF("CBIFONT", "\Cj%S", Language("LITH_TXT_INFO_TITLE_%S", bip->curpage->name));
         HudMessagePlain(hid--, 200.4, 45.1, TICSECOND);
         HudMessageF("CBIFONT", "%S", Language("LITH_TXT_INFO_DESCR_%S", bip->curpage->name));
         HudMessagePlain(hid--, 111.1, 60.1, TICSECOND);
         
         ACS_SetHudClipRect(0, 0, 0, 0);
      }
      
      if(GUI_Button(GUI_ID("bBBT"), gst, &hid, 20, 38, "<BACK", false, &backbtnp))
         bip->curcategory = BIPC_MAIN;
      
      avail = bip->categoryavail[bip->curcategory];
      max   = bip->categorymax[bip->curcategory];
   }
   
   DrawSpriteAlpha("lgfx/UI/bip.png", hid--, 20.1, 30.1, TICSECOND, 0.6);
   HudMessageF("CBIFONT", "BIOTIC INFORMATION PANEL ver2.5");
   HudMessagePlain(hid--, 35.1, 30.1, TICSECOND);
   
   HudMessageF("CBIFONT", "%i/%i AVAILABLE", avail, max);
   HudMessagePlain(hid--, 300.2, 30.1, TICSECOND);
   
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
   player_t *p = &players[ACS_PlayerNumber()];
   
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

