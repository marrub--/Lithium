// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#ifndef LITH_SHOPDEF_H
#define LITH_SHOPDEF_H

enum
{
   shoptype_items,
   shoptype_upgrades,
   shoptype_max
};

typedef struct shopdef
{
   __str name;
   __str bipunlock;
   i96   cost;

   void (*shopBuy)   (struct player *p, struct shopdef const *def, void *obj);
   bool (*shopCanBuy)(struct player *p, struct shopdef const *def, void *obj);
   bool (*shopGive)  (struct player *p, struct shopdef const *def, void *obj, int tid);
} shopdef_t;

i96 Lith_ShopGetCost(struct player *p, shopdef_t const *def);
bool Lith_ShopCanBuy(struct player *p, shopdef_t const *def, void *obj);
[[__optional_args(1)]] bool Lith_ShopBuy(struct player *p, shopdef_t const *def, void *obj, __str namefmt, bool nodelivery, bool nolog);

#endif

