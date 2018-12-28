// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifndef p_shopdef_h
#define p_shopdef_h

enum
{
   shoptype_items,
   shoptype_upgrades,
   shoptype_max
};

typedef struct shopdef
{
   str name;
   str bipunlock;
   i96   cost;

   void (*shopBuy)   (struct player *p, struct shopdef const *def, void *obj);
   bool (*shopCanBuy)(struct player *p, struct shopdef const *def, void *obj);
   bool (*shopGive)  (struct player *p, struct shopdef const *def, void *obj, i32 tid);
} shopdef_t;

i96 Lith_ShopGetCost(struct player *p, shopdef_t const *def);
bool Lith_ShopCanBuy(struct player *p, shopdef_t const *def, void *obj);
optargs(1) bool Lith_ShopBuy(struct player *p, shopdef_t const *def, void *obj, char const *namefmt, bool nodelivery, bool nolog);

#endif
