// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
// vim: columns=120
#include "lith_common.h"
#include "lith_player.h"

// Types ---------------------------------------------------------------------|

enum {
   sif_weapon = 1 << 0,
};

typedef struct shopitem_s
{
   [[__anonymous]] shopdef_t shopdef;
   int pclass;
   int count;
   __str classname;
   int flags;
} shopitem_t;

// Static Objects ------------------------------------------------------------|

static shopitem_t shopitems[] = {
// {{"Name-----------", "BIP------------", Cost---}, pcl_name,     Cnt-, "Class---------------", [Flags]   },
   {{"RocketAmmo",      null,              9000   }, pcl_any,      5,    "Lith_RocketAmmo"                 },
   {{"PlasmaAmmo",      null,              75750  }, pcl_any,      1000, "Lith_PlasmaAmmo"                 },
   {{"ChargeFist",      "ChargeFist",      100000 }, pcl_marine,   1,    "Lith_ChargeFist",      sif_weapon},
   {{"Revolver",        "Revolver",        500000 }, pcl_marine,   1,    "Lith_Revolver",        sif_weapon},
   {{"LazShotgun",      "LazShotgun",      1800000}, pcl_marine,   1,    "Lith_LazShotgun",      sif_weapon},
   {{"SniperRifle",     "SniperRifle",     1800000}, pcl_marine,   1,    "Lith_SniperRifle",     sif_weapon},
   {{"MissileLauncher", "MissileLauncher", 2500000}, pcl_outcasts, 1,    "Lith_MissileLauncher", sif_weapon},
   {{"PlasmaDiffuser",  "PlasmaDiffuser",  2500000}, pcl_outcasts, 1,    "Lith_PlasmaDiffuser",  sif_weapon},
   {{"Allmap",          null,              100000 }, pcl_any,      1,    "Allmap"                          },
   {{"Infrared",        null,              70000  }, pcl_any,      1,    "Infrared"                        },
   {{"RadSuit",         null,              100000 }, pcl_any,      1,    "RadSuit"                         },
};

// Static Functions ----------------------------------------------------------|

//
// Shop_CanBuy
//
static bool Shop_CanBuy(player_t *p, shopdef_t const *, void *item_)
{
   shopitem_t *item = item_;
   int cur =  ACS_CheckInventory(item->classname);
   int max = ACS_GetMaxInventory(0, item->classname);
   return max == 0 || cur < max;
}

//
// Shop_Buy
//
static void Shop_Buy(player_t *p, shopdef_t const *, void *item_)
{
   shopitem_t *item = item_;
   p->itemsbought++;
   ACS_GiveInventory(item->classname, item->count);
}

//
// Shop_Give
//
static bool Shop_Give(player_t *p, shopdef_t const *, void *item_, int tid)
{
   shopitem_t *item = item_;
   p->itemsbought++;
   if(item->flags & sif_weapon) {
      Lith_GiveActorInventory(p->tid, item->classname, item->count);
      return false;
   } else {
      Lith_GiveActorInventory(tid, item->classname, item->count);
      return true;
   }
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_GInit_Shop
//
void Lith_GInit_Shop(void)
{
   for(int i = 0; i < countof(shopitems); i++)
   {
      shopitem_t *info = &shopitems[i];
      info->shopBuy    = Shop_Buy;
      info->shopCanBuy = Shop_CanBuy;
      info->shopGive   = Shop_Give;
   }
}

//
// Lith_CBITab_Shop
//
void Lith_CBITab_Shop(gui_state_t *g, player_t *p)
{
   int nitems = 0;
   for(int i = 0; i < countof(shopitems); i++) {
      if(shopitems[i].pclass & p->pclass)
         nitems++;
   }

   Lith_GUI_ScrollBegin(g, st_shopscr, 15, 30, guipre.btnlist.w, 192, guipre.btnlist.h * nitems);

   for(int i = 0, y = 0; i < countof(shopitems); i++)
   {
      if(Lith_GUI_ScrollOcclude(g, st_shopscr, y, guipre.btnlistsel.h) || !(shopitems[i].pclass & p->pclass))
         continue;

      __str name = Language("LITH_TXT_SHOP_TITLE_%S", shopitems[i].name);

      if(Lith_GUI_Button_Id(g, i, name, 0, y, i == g->st[st_shopsel].i, .preset = &guipre.btnlistsel))
         g->st[st_shopsel].i = i;

      y += guipre.btnlistsel.h;
   }

   Lith_GUI_ScrollEnd(g, st_shopscr);

   shopitem_t *item = &shopitems[g->st[st_shopsel].i];

   ACS_SetHudClipRect(111, 30, 184, 150, 184);

   HudMessageF("CBIFONT", "%LS: %S\Cnscr", "LITH_COST", Lith_ScoreSep(p->getCost(&item->shopdef)));
   HudMessagePlain(g->hid--, 111.1, 30.1, TICSECOND);

   HudMessageF("CBIFONT", "%S", Language("LITH_TXT_SHOP_DESCR_%S", item->name));
   HudMessageParams(HUDMSG_PLAIN, g->hid--, CR_WHITE, 111.1, 40.1, TICSECOND);

   ACS_SetHudClipRect(0, 0, 0, 0);

   if(Lith_GUI_Button(g, "Buy", 259, 170, !p->canBuy(&item->shopdef, item)))
      p->buy(&item->shopdef, item, "LITH_TXT_SHOP_TITLE_%S", false);
}

// EOF

