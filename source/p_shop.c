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

static
struct shopitem shopitems[] = {
/* {{"Name-----------", "BIP------------", Cost---}, Class, Cnt-, "Class---------------", [Flags]}, */
   {{"ChargeFist",      "ChargeFist",       100000}, gO, 1, sOBJ "ChargeFist",      true},
   {{"Revolver",        "Revolver",         500000}, pM, 1, sOBJ "Revolver",        true},
   {{"LazShotgun",      "LazShotgun",      1800000}, pM, 1, sOBJ "LazShotgun",      true},
   {{"SniperRifle",     "SniperRifle",     1800000}, pM, 1, sOBJ "SniperRifle",     true},
   {{"MissileLauncher", "MissileLauncher", 2500000}, gO, 1, sOBJ "MissileLauncher", true},
   {{"PlasmaDiffuser",  "PlasmaDiffuser",  2500000}, gO, 1, sOBJ "PlasmaDiffuser",  true},
};

/* Static Functions -------------------------------------------------------- */

static
bool Shop_CanBuy(struct shopdef const *, void *item_)
{
   struct shopitem *item = item_;
   i32 cur = InvNum(item->classname);
   i32 max = InvMax(item->classname);
   return max == 0 || cur < max;
}

static
void Shop_Buy(struct shopdef const *, void *item_)
{
   struct shopitem *item = item_;
   pl.itemsbought++;
   InvGive(item->classname, item->count);
}

static
bool Shop_Give(struct shopdef const *, void *item_, i32 tid)
{
   struct shopitem *item = item_;
   pl.itemsbought++;
   if(item->weapon) {
      PtrInvGive(pl.tid, item->classname, item->count);
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

void P_CBI_TabShop(struct gui_state *g)
{
   i32 nitems = 0;
   for(i32 i = 0; i < countof(shopitems); i++) {
      if(shopitems[i].pclass & pl.pclass)
         nitems++;
   }

   G_ScrBeg(g, &CBIState(g)->shopscr, 2, 23, gui_p.btnlist.w, 186, gui_p.btnlist.h * nitems);

   for(i32 i = 0, y = 0; i < countof(shopitems); i++)
   {
      if(G_ScrOcc(g, &CBIState(g)->shopscr, y, gui_p.btnlistsel.h) || !(shopitems[i].pclass & pl.pclass))
         continue;

      i32 *shopsel = &CBIState(g)->shopsel;
      if(G_Button_HId(g, i, tmpstr(lang_fmt(LANG "SHOP_TITLE_%s",
                                            shopitems[i].name)),
                      0, y, i == *shopsel, Pre(btnlistsel)))
      {
         *shopsel = i;
      }

      y += gui_p.btnlistsel.h;
   }

   G_ScrEnd(g, &CBIState(g)->shopscr);

   struct shopitem *item = &shopitems[CBIState(g)->shopsel];

   G_Clip(g, g->ox+98, g->oy+17, 190, 170, 184);

   PrintTextFmt("%s\Cnscr", scoresep(P_Shop_Cost(&item->shopdef)));
   PrintText(sf_smallfnt, g->defcr, g->ox+98,1, g->oy+17,1);

   PrintText_str(ns(lang_fmt(LANG "SHOP_DESCR_%s", item->name)), sf_smallfnt, g->defcr, g->ox+98,1, g->oy+27,1);

   G_ClipRelease(g);

   if(G_Button(g, tmpstr(lang(sl_buy)), 98, 192, !P_Shop_CanBuy(&item->shopdef, item), .fill = &CBIState(g)->buyfill))
      P_Shop_Buy(&item->shopdef, item, LANG "SHOP_TITLE_%s", false);
}

/* EOF */
