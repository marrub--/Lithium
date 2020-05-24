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

static struct shopitem shopitems[] = {
/* {{"Name-----------", "BIP------------", Cost---}, Class, Cnt-, "Class---------------", [Flags]}, */
   {{s"RocketAmmo", snil, 9000},  gA, 5,    sOBJ "RocketAmmo"},
   {{s"PlasmaAmmo", snil, 75750}, gA, 1000, sOBJ "PlasmaAmmo"},

   {{s"ChargeFist",      s"ChargeFist",       100000}, pM, 1, sOBJ "ChargeFist",      true},
   {{s"Revolver",        s"Revolver",         500000}, pM, 1, sOBJ "Revolver",        true},
   {{s"LazShotgun",      s"LazShotgun",      1800000}, pM, 1, sOBJ "LazShotgun",      true},
   {{s"SniperRifle",     s"SniperRifle",     1800000}, pM, 1, sOBJ "SniperRifle",     true},
   {{s"MissileLauncher", s"MissileLauncher", 2500000}, gO, 1, sOBJ "MissileLauncher", true},
   {{s"PlasmaDiffuser",  s"PlasmaDiffuser",  2500000}, gO, 1, sOBJ "PlasmaDiffuser",  true},

   {{s"Allmap",   snil, 100000}, gA, 1, s"Allmap"},
   {{s"Infrared", snil, 70000},  gA, 1, s"Infrared"},
   {{s"RadSuit",  snil, 100000}, gA, 1, s"RadSuit"},
};

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

   G_ScrBeg(g, &CBIState(g)->shopscr, 15, 36, gui_p.btnlist.w, 186, gui_p.btnlist.h * nitems);

   for(i32 i = 0, y = 0; i < countof(shopitems); i++)
   {
      if(G_ScrOcc(g, &CBIState(g)->shopscr, y, gui_p.btnlistsel.h) || !(shopitems[i].pclass & p->pclass))
         continue;

      cstr name = LanguageC(LANG "SHOP_TITLE_%S", shopitems[i].name);

      i32 *shopsel = &CBIState(g)->shopsel;
      if(G_Button_HId(g, i, name, 0, y, i == *shopsel, Pre(btnlistsel)))
         *shopsel = i;

      y += gui_p.btnlistsel.h;
   }

   G_ScrEnd(g, &CBIState(g)->shopscr);

   struct shopitem *item = &shopitems[CBIState(g)->shopsel];

   SetClipW(111, 30, 184, 150, 184);

   PrintTextFmt("%s %s\Cnscr", LC(LANG "COST"), scoresep(P_Shop_Cost(p, &item->shopdef)));
   PrintText(s_smallfnt, g->defcr, 111,1, 30,1);

   PrintText_str(Language(LANG "SHOP_DESCR_%S", item->name), s_smallfnt, g->defcr, 111,1, 40,1);

   ClearClip();

   if(G_Button(g, LC(LANG "BUY"), 259, 170, !P_Shop_CanBuy(p, &item->shopdef, item), .fill = {&CBIState(g)->buyfill, p->getCVarI(sc_gui_buyfiller)}))
      P_Shop_Buy(p, &item->shopdef, item, LANG "SHOP_TITLE_%S", false);
}

/* EOF */
