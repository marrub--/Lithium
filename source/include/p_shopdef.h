/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Unified shop definitions.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef p_shopdef_h
#define p_shopdef_h

enum
{
   shoptype_items,
   shoptype_upgrades,
   shoptype_max
};

struct shopdef
{
   cstr name;
   cstr bipunlock;
   i96  cost;

   void (*ShopBuy)   (struct shopdef const *def, void *obj);
   bool (*ShopCanBuy)(struct shopdef const *def, void *obj);
   bool (*ShopGive)  (struct shopdef const *def, void *obj, i32 tid);
};

void Shop_MInit(void);

i96 P_Shop_Cost(struct shopdef const *def);
bool P_Shop_CanBuy(struct shopdef const *def, void *obj);
optargs(1) bool P_Shop_Buy(struct shopdef const *def, void *obj, cstr namefmt, bool nodelivery, bool nolog);

#endif
