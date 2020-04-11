/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Player shop handling.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"

/* Types ------------------------------------------------------------------- */

struct shopitem
{
   anonymous struct shopdef shopdef;
   i32 pclass;
   i32 count;
   str classname;
   bool weapon;
};

/* Static Objects ---------------------------------------------------------- */

StrEntON
static struct shopitem shopitems[] = {
/* {{"Name-----------", "BIP------------", Cost---}, Class, Cnt-, "Class---------------", [Flags]}, */
   {{"RocketAmmo", snil, 9000},  gA, 5,    OBJ "RocketAmmo"},
   {{"PlasmaAmmo", snil, 75750}, gA, 1000, OBJ "PlasmaAmmo"},

   {{"ChargeFist",      "ChargeFist",       100000}, pM, 1, OBJ "ChargeFist",      true},
   {{"Revolver",        "Revolver",         500000}, pM, 1, OBJ "Revolver",        true},
   {{"LazShotgun",      "LazShotgun",      1800000}, pM, 1, OBJ "LazShotgun",      true},
   {{"SniperRifle",     "SniperRifle",     1800000}, pM, 1, OBJ "SniperRifle",     true},
   {{"MissileLauncher", "MissileLauncher", 2500000}, gO, 1, OBJ "MissileLauncher", true},
   {{"PlasmaDiffuser",  "PlasmaDiffuser",  2500000}, gO, 1, OBJ "PlasmaDiffuser",  true},

   {{"Allmap",   snil, 100000}, gA, 1, "Allmap"},
   {{"Infrared", snil, 70000},  gA, 1, "Infrared"},
   {{"RadSuit",  snil, 100000}, gA, 1, "RadSuit"},
};
StrEntOFF

/* Static Functions -------------------------------------------------------- */

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
   if(item->weapon) {
      PtrInvGive(p->tid, item->classname, item->count);
      return false;
   } else {
      PtrInvGive(tid, item->classname, item->count);
      return true;
   }
}

/* Extern Functions -------------------------------------------------------- */

void Shop_MInit(void)
{
   for(i32 i = 0; i < countof(shopitems); i++)
   {
      struct shopitem *info = &shopitems[i];
      info->ShopBuy    = Shop_Buy;
      info->ShopCanBuy = Shop_CanBuy;
      info->ShopGive   = Shop_Give;
   }
}

void P_CBI_TabShop(struct gui_state *g, struct player *p)
{
   i32 nitems = 0;
   for(i32 i = 0; i < countof(shopitems); i++) {
      if(shopitems[i].pclass & p->pclass)
         nitems++;
   }

   G_ScrollBegin(g, &CBIState(g)->shopscr, 15, 36, gui_p.btnlist.w, 186, gui_p.btnlist.h * nitems);

   for(i32 i = 0, y = 0; i < countof(shopitems); i++)
   {
      if(G_ScrollOcclude(g, &CBIState(g)->shopscr, y, gui_p.btnlistsel.h) || !(shopitems[i].pclass & p->pclass))
         continue;

      cstr name = LanguageC(LANG "SHOP_TITLE_%S", shopitems[i].name);

      i32 *shopsel = &CBIState(g)->shopsel;
      if(G_Button_Id(g, i, name, 0, y, i == *shopsel, Pre(btnlistsel)))
         *shopsel = i;

      y += gui_p.btnlistsel.h;
   }

   G_ScrollEnd(g, &CBIState(g)->shopscr);

   struct shopitem *item = &shopitems[CBIState(g)->shopsel];

   SetClipW(111, 30, 184, 150, 184);

   PrintTextFmt("%s %s\Cnscr", LC(LANG "COST"), scoresep(P_Shop_Cost(p, &item->shopdef)));
   PrintText(s_smallfnt, g->defcr, 111,1, 30,1);

   PrintText_str(Language(LANG "SHOP_DESCR_%S", item->name), s_smallfnt, g->defcr, 111,1, 40,1);

   ClearClip();

   if(G_Button(g, LC(LANG "BUY"), 259, 170, !P_Shop_CanBuy(p, &item->shopdef, item)))
      P_Shop_Buy(p, &item->shopdef, item, LANG "SHOP_TITLE_%S", false);
}

/* EOF */
