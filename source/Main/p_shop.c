// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "lith_common.h"
#include "lith_player.h"

StrEntON

// Types ---------------------------------------------------------------------|

enum {
   sif_weapon = 1 << 0,
};

typedef struct shopitem_s
{
   anonymous shopdef_t shopdef;
   int pclass;
   int count;
   __str classname;
   int flags;
} shopitem_t;

// Static Objects ------------------------------------------------------------|

static shopitem_t shopitems[] = {
// {{"Name-----------", "BIP------------", Cost---}, Class, Cnt-, "Class---------------", [Flags]},
   {{"RocketAmmo", null, 9000},  gA, 5,    OBJ "RocketAmmo"},
   {{"PlasmaAmmo", null, 75750}, gA, 1000, OBJ "PlasmaAmmo"},

   {{"ChargeFist",      "ChargeFist",      100000},  pM, 1,    OBJ "ChargeFist",      sif_weapon},
   {{"Revolver",        "Revolver",        500000},  pM, 1,    OBJ "Revolver",        sif_weapon},
   {{"LazShotgun",      "LazShotgun",      1800000}, pM, 1,    OBJ "LazShotgun",      sif_weapon},
   {{"SniperRifle",     "SniperRifle",     1800000}, pM, 1,    OBJ "SniperRifle",     sif_weapon},
   {{"MissileLauncher", "MissileLauncher", 2500000}, gO, 1,    OBJ "MissileLauncher", sif_weapon},
   {{"PlasmaDiffuser",  "PlasmaDiffuser",  2500000}, gO, 1,    OBJ "PlasmaDiffuser",  sif_weapon},

   {{"Allmap",   null, 100000}, gA, 1, "Allmap"},
   {{"Infrared", null, 70000},  gA, 1, "Infrared"},
   {{"RadSuit",  null, 100000}, gA, 1, "RadSuit"},
};

// Static Functions ----------------------------------------------------------|

static bool Shop_CanBuy(struct player *p, shopdef_t const *, void *item_)
{
   shopitem_t *item = item_;
   int cur = InvNum(item->classname);
   int max = InvMax(item->classname);
   return max == 0 || cur < max;
}

static void Shop_Buy(struct player *p, shopdef_t const *, void *item_)
{
   shopitem_t *item = item_;
   p->itemsbought++;
   InvGive(item->classname, item->count);
}

static bool Shop_Give(struct player *p, shopdef_t const *, void *item_, int tid)
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

void Lith_CBITab_Shop(gui_state_t *g, struct player *p)
{
   int nitems = 0;
   for(int i = 0; i < countof(shopitems); i++) {
      if(shopitems[i].pclass & p->pclass)
         nitems++;
   }

   Lith_GUI_ScrollBegin(g, &CBIState(g)->shopscr, 15, 36, guipre.btnlist.w, 186, guipre.btnlist.h * nitems);

   for(int i = 0, y = 0; i < countof(shopitems); i++)
   {
      if(Lith_GUI_ScrollOcclude(g, &CBIState(g)->shopscr, y, guipre.btnlistsel.h) || !(shopitems[i].pclass & p->pclass))
         continue;

      char const *name = LanguageC(cLANG "SHOP_TITLE_%S", shopitems[i].name);

      int *shopsel = &CBIState(g)->shopsel;
      if(Lith_GUI_Button_Id(g, i, name, 0, y, i == *shopsel, Pre(btnlistsel)))
         *shopsel = i;

      y += guipre.btnlistsel.h;
   }

   Lith_GUI_ScrollEnd(g, &CBIState(g)->shopscr);

   shopitem_t *item = &shopitems[CBIState(g)->shopsel];

   SetClipW(111, 30, 184, 150, 184);

   PrintTextFmt("%LS %S\Cnscr", LANG "COST", scoresep(p->getCost(&item->shopdef)));
   PrintText("cbifont", CR_WHITE, 111,1, 30,1);

   PrintTextStr(Language(LANG "SHOP_DESCR_%S", item->name));
   PrintText("cbifont", CR_WHITE, 111,1, 40,1);

   ClearClip();

   if(Lith_GUI_Button(g, LC(cLANG "BUY"), 259, 170, !p->canBuy(&item->shopdef, item)))
      p->buy(&item->shopdef, item, LANG "SHOP_TITLE_%S", false);
}
#endif

// EOF
