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
   {"CannonAmmo",   113620    ,   1, "Lith_CannonAmmo"      },
   {"Revolver",     500000    ,   1, "Lith_Revolver"        },
   {"Allmap",       100000    ,   1, "Allmap"               },
   {"Berserk",      150000    ,   1, "Berserk"              },
   {"BlurSphere",   70000     ,   1, "BlurSphere"           },
   {"Infrared",     70000     ,   1, "Infrared"             },
   {"RadSuit",      100000    ,   1, "RadSuit"              },
// {"DivSigil",     7772940   ,   1, "Lith_DivisionSigil"   },
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
   
   Lith_LogF(p, "> Bought %S", Language("LITH_TXT_SHOP_TITLE_%S", def->name));
   
   Lith_UnlockBIPPage(&p->bip, def->name);
   
   ACS_GiveInventory(def->class, def->count);
   Lith_TakeScore(p, Lith_PlayerDiscount(def->cost));
   p->itemsbought++;
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_CBITab_Shop
//
void Lith_CBITab_Shop(gui_state_t *g, player_t *p)
{
   Lith_GUI_ScrollBegin(g, st_shopscr, 15, 30, btnlist.w, 152, btnlist.h * shopdefsnum);
   
   for(int i = 0; i < shopdefsnum; i++)
   {
      int y = btnlist.h * i;
      
      if(Lith_GUI_ScrollOcclude(g, st_shopscr, y, btnlist.h))
         continue;
      
      __str name = Language("LITH_TXT_SHOP_TITLE_%S", shopdefs[i].name);
      
      if(Lith_GUI_Button_Id(g, i, name, 0, y, i == g->st[st_shopsel].i, .preset = &btnlist))
         g->st[st_shopsel].i = i;
   }
   
   Lith_GUI_ScrollEnd(g, st_shopscr);
   
   shopdef_t *def = &shopdefs[g->st[st_shopsel].i];
   
   ACS_SetHudClipRect(111, 30, 184, 150, 184);
   
   HudMessageF("CBIFONT", "%LS: %lli\Cnscr", "LITH_COST", def->cost);
   HudMessagePlain(g->hid--, 111.1, 30.1, TICSECOND);
   
   HudMessageF("CBIFONT", "%S", Language("LITH_TXT_SHOP_DESCR_%S", def->name));
   HudMessagePlain(g->hid--, 111.1, 40.1, TICSECOND);
   
   ACS_SetHudClipRect(0, 0, 0, 0);
   
   if(Lith_GUI_Button(g, "Buy", 259, 170, !Shop_CanBuy(p, def)))
      Shop_Buy(p, def);
}

// EOF

