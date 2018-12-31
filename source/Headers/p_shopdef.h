// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
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
   str name;
   str bipunlock;
   i96 cost;

   void (*ShopBuy)   (struct player *p, struct shopdef const *def, void *obj);
   bool (*ShopCanBuy)(struct player *p, struct shopdef const *def, void *obj);
   bool (*ShopGive)  (struct player *p, struct shopdef const *def, void *obj, i32 tid);
};

i96 Lith_ShopGetCost(struct player *p, struct shopdef const *def);
bool Lith_ShopCanBuy(struct player *p, struct shopdef const *def, void *obj);
optargs(1) bool Lith_ShopBuy(struct player *p, struct shopdef const *def, void *obj, char const *namefmt, bool nodelivery, bool nolog);

#endif
