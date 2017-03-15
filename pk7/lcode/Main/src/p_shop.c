#include "lith_common.h"
#include "lith_player.h"


//----------------------------------------------------------------------------
// Types
//

typedef struct shopitem_s
{
   [[__anonymous]] shopdef_t shopdef;
   int count;
   __str classname;
} shopitem_t;


//----------------------------------------------------------------------------
// Static Objects
//

static shopitem_t shopitems[] = {
// {{"Name-----------", "BIP------------", Cost---}, Cnt-, "Class----------------"},
   {{"RocketAmmo",      null,              9000   },    5, "Lith_RocketAmmo"      },
   {{"PlasmaAmmo",      null,              75750  }, 1000, "Lith_PlasmaAmmo"      },
   {{"Revolver",        "Revolver",        500000 },    1, "Lith_Revolver"        },
   {{"LazShotgun",      "LazShotgun",      1800000},    1, "Lith_LazShotgun"      },
   {{"SniperRifle",     "SniperRifle",     1800000},    1, "Lith_SniperRifle"     },
   {{"MissileLauncher", "MissileLauncher", 2500000},    1, "Lith_MissileLauncher" },
// {{"Gameboy",         null,              10000  },    1, "Lith_Gameboy"         },
   {{"Allmap",          null,              100000 },    1, "Allmap"               },
   {{"Infrared",        null,              70000  },    1, "Infrared"             },
   {{"RadSuit",         null,              100000 },    1, "RadSuit"              },
// {{"DivSigil",        "DivSigil",        7772940},    1, "Lith_DivisionSigil"   },
};

static size_t const shopitemsnum = countof(shopitems);


//----------------------------------------------------------------------------
// Static Functions
//

//
// Shop_CanBuy
//
static bool Shop_CanBuy(player_t *p, shopdef_t const *def, void *item_)
{
   shopitem_t *item = item_;
   int cur =  ACS_CheckInventory(item->classname);
   int max = ACS_GetMaxInventory(0, item->classname);
   return max == 0 || cur < max;
}

//
// Shop_Buy
//
static void Shop_Buy(player_t *p, shopdef_t const *def, void *item_)
{
   shopitem_t *item = item_;
   ACS_GiveInventory(item->classname, item->count);
   p->itemsbought++;
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_GSInit_Shop
//
void Lith_GSInit_Shop(void)
{
   for(int i = 0; i < shopitemsnum; i++)
   {
      shopitem_t *info = &shopitems[i];
      info->shopBuy    = Shop_Buy;
      info->shopCanBuy = Shop_CanBuy;
   }
}

//
// Lith_CBITab_Shop
//
void Lith_CBITab_Shop(gui_state_t *g, player_t *p)
{
   Lith_GUI_ScrollBegin(g, st_shopscr, 15, 30, btnlist.w, 192, btnlist.h * shopitemsnum);
   
   for(int i = 0; i < shopitemsnum; i++)
   {
      int y = btnlist.h * i;
      
      if(Lith_GUI_ScrollOcclude(g, st_shopscr, y, btnlist.h))
         continue;
      
      __str name = Language("LITH_TXT_SHOP_TITLE_%S", shopitems[i].name);
      
      if(Lith_GUI_Button_Id(g, i, name, 0, y, i == g->st[st_shopsel].i, .preset = &btnlistsel))
         g->st[st_shopsel].i = i;
   }
   
   Lith_GUI_ScrollEnd(g, st_shopscr);
   
   shopitem_t *item = &shopitems[g->st[st_shopsel].i];
   
   ACS_SetHudClipRect(111, 30, 184, 150, 184);
   
   HudMessageF("CBIFONT", "%LS: %S\Cnscr", "LITH_COST", Lith_ScoreSep(Lith_ShopGetCost(p, &item->shopdef)));
   HudMessagePlain(g->hid--, 111.1, 30.1, TICSECOND);
   
   HudMessageF("CBIFONT", "%S", Language("LITH_TXT_SHOP_DESCR_%S", item->name));
   HudMessageParams(HUDMSG_PLAIN, g->hid--, CR_WHITE, 111.1, 40.1, TICSECOND);
   
   ACS_SetHudClipRect(0, 0, 0, 0);
   
   if(Lith_GUI_Button(g, "Buy", 259, 170, !Lith_ShopCanBuy(p, &item->shopdef, item)))
      Lith_ShopBuy(p, &item->shopdef, item, "LITH_TXT_SHOP_TITLE_%S");
}

// EOF

