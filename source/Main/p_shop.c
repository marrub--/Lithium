// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "common.h"
#include "p_player.h"

// Types ---------------------------------------------------------------------|

enum {
   sif_weapon = 1 << 0,
};

struct shopitem
{
   anonymous struct shopdef shopdef;
   i32 pclass;
   i32 count;
   str classname;
   i32 flags;
};

// Static Objects ------------------------------------------------------------|

StrEntON
static struct shopitem shopitems[] = {
// {{"Name-----------", "BIP------------", Cost---}, Class, Cnt-, "Class---------------", [Flags]},
   {{"RocketAmmo", nil, 9000},  gA, 5,    OBJ "RocketAmmo"},
   {{"PlasmaAmmo", nil, 75750}, gA, 1000, OBJ "PlasmaAmmo"},

   {{"ChargeFist",      "ChargeFist",       100000}, pM, 1, OBJ "ChargeFist",      sif_weapon},
   {{"Revolver",        "Revolver",         500000}, pM, 1, OBJ "Revolver",        sif_weapon},
   {{"LazShotgun",      "LazShotgun",      1800000}, pM, 1, OBJ "LazShotgun",      sif_weapon},
   {{"SniperRifle",     "SniperRifle",     1800000}, pM, 1, OBJ "SniperRifle",     sif_weapon},
   {{"MissileLauncher", "MissileLauncher", 2500000}, gO, 1, OBJ "MissileLauncher", sif_weapon},
   {{"PlasmaDiffuser",  "PlasmaDiffuser",  2500000}, gO, 1, OBJ "PlasmaDiffuser",  sif_weapon},

   {{"Allmap",   nil, 100000}, gA, 1, "Allmap"},
   {{"Infrared", nil, 70000},  gA, 1, "Infrared"},
   {{"RadSuit",  nil, 100000}, gA, 1, "RadSuit"},
};
StrEntOFF

// Static Functions ----------------------------------------------------------|

static bool Shop_CanBuy(struct player *p, struct shopdef const *, void *item_)
{
   struct shopitem *item = item_;
   i32 cur = InvNum(item->classname);
   i32 max = InvMax(item->classname);
   return max == 0 || cur < max;
}

static void Shop_Buy(struct player *p, struct shopdef const *, void *item_)
{
   struct shopitem *item = item_;
   p->itemsbought++;
   InvGive(item->classname, item->count);
}

static bool Shop_Give(struct player *p, struct shopdef const *, void *item_, i32 tid)
{
   struct shopitem *item = item_;
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

void Lith_GInit_Shop(void)
{
   for(i32 i = 0; i < countof(shopitems); i++)
   {
      struct shopitem *info = &shopitems[i];
      info->ShopBuy    = Shop_Buy;
      info->ShopCanBuy = Shop_CanBuy;
      info->ShopGive   = Shop_Give;
   }
}

void Lith_CBITab_Shop(struct gui_state *g, struct player *p)
{
   i32 nitems = 0;
   for(i32 i = 0; i < countof(shopitems); i++) {
      if(shopitems[i].pclass & p->pclass)
         nitems++;
   }

   Lith_GUI_ScrollBegin(g, &CBIState(g)->shopscr, 15, 36, gui_p.btnlist.w, 186, gui_p.btnlist.h * nitems);

   for(i32 i = 0, y = 0; i < countof(shopitems); i++)
   {
      if(Lith_GUI_ScrollOcclude(g, &CBIState(g)->shopscr, y, gui_p.btnlistsel.h) || !(shopitems[i].pclass & p->pclass))
         continue;

      char const *name = LanguageC(LANG "SHOP_TITLE_%S", shopitems[i].name);

      i32 *shopsel = &CBIState(g)->shopsel;
      if(Lith_GUI_Button_Id(g, i, name, 0, y, i == *shopsel, Pre(btnlistsel)))
         *shopsel = i;

      y += gui_p.btnlistsel.h;
   }

   Lith_GUI_ScrollEnd(g, &CBIState(g)->shopscr);

   struct shopitem *item = &shopitems[CBIState(g)->shopsel];

   SetClipW(111, 30, 184, 150, 184);

   PrintTextFmt("%s %S\Cnscr", LC(LANG "COST"), scoresep(p->getCost(&item->shopdef)));
   PrintText(s_cbifont, CR_WHITE, 111,1, 30,1);

   PrintText_str(Language(LANG "SHOP_DESCR_%S", item->name), s_cbifont, CR_WHITE, 111,1, 40,1);

   ClearClip();

   if(Lith_GUI_Button(g, LC(LANG "BUY"), 259, 170, !p->canBuy(&item->shopdef, item)))
      p->buy(&item->shopdef, item, LANG "SHOP_TITLE_%S", false);
}
#endif

// EOF
