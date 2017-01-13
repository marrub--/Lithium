#include "lith_common.h"
#include "lith_player.h"


//----------------------------------------------------------------------------
// Type Definitions
//

typedef struct shopdef_s
{
   __str name;
   score_t cost;
   int count;
   __str class;
} shopdef_t;


//----------------------------------------------------------------------------
// Static Objects
//

static shopdef_t shopdefs[] = {
// {"Name--------", Cost------, Cnt, Class------------------},
   {"RocketAmmo",   9000      ,   5, "Lith_RocketAmmo"      },
   {"PlasmaAmmo",   75750     ,1000, "Lith_PlasmaAmmo"      },
   {"CannonAmmo",   113625    ,   1, "Lith_CannonAmmo"      },
   {"Allmap",       100000    ,   1, "Allmap"               },
   {"Berserk",      150000    ,   1, "Berserk"              },
   {"BlurSphere",   70000     ,   1, "BlurSphere"           },
   {"Infrared",     70000     ,   1, "Infrared"             },
   {"RadSuit",      100000    ,   1, "RadSuit"              },
// {"DivSigil",     7772944   ,   1, "Lith_DivisionSigil"   },
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
   return (max == 0 || cur < max) && p->score - Lith_PlayerDiscount(def->cost) >= 0;
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
   Lith_TakeScore(p, Lith_PlayerDiscount(def->cost));
   p->itemsbought++;
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// CBI_Tab_Shop
//
int CBI_Tab_Shop(player_t *p, int hid, cbi_t *cbi, gui_state_t *gst)
{
   GUI_BEGIN_LIST(GUI_ID("sSCL"), gst, &hid, 20, 30, 152, &cbi->gst.scrlst[CBI_SCRLST_SHOP]);
   
   for(int i = 0; i < shopdefsnum; i++)
   {
      GUI_LIST_OFFSETS(i, shopdefsnum, 152, cbi->gst.scrlst[CBI_SCRLST_SHOP]);
      
      __str id   = StrParam("sN%.2i", i);
      __str name = Language("LITH_TXT_SHOP_TITLE_%S", shopdefs[i].name);
      
      if(GUI_Button(GUI_ID(id), gst, &hid, 0, addy, name, cbi->gst.lstst[CBI_LSTST_SHOP] == i, &gui_listbtnparm))
         cbi->gst.lstst[CBI_LSTST_SHOP] = i;
   }
   
   GUI_END_LIST(gst);
   
   shopdef_t *def = &shopdefs[cbi->gst.lstst[CBI_LSTST_SHOP]];
   
   ACS_SetHudClipRect(111, 30, 184, 150, 184);
   
   HudMessageF("CBIFONT", "%LS: %lli\Cnscr", "LITH_COST", def->cost);
   HudMessagePlain(hid--, 111.1, 30.1, TICSECOND);
   
   HudMessageF("CBIFONT", "%S", Language("LITH_TXT_SHOP_DESCR_%S", def->name));
   HudMessagePlain(hid--, 111.1, 40.1, TICSECOND);
   
   ACS_SetHudClipRect(0, 0, 0, 0);
   
   if(GUI_Button(GUI_ID("sBUY"), gst, &hid, 259, 170, "Buy", !Shop_CanBuy(p, def)))
      Shop_Buy(p, def);
   
   return hid;
}

// EOF

