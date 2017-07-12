#ifndef LITH_SHOPDEF_H
#define LITH_SHOPDEF_H

enum
{
   shoptype_items,
   shoptype_upgrades,
   shoptype_max
};

score_t Lith_ShopGetCost(struct player *p, shopdef_t const *def);
bool Lith_ShopCanBuy    (struct player *p, shopdef_t const *def, void *obj);
bool Lith_ShopBuy       (struct player *p, shopdef_t const *def, void *obj, __str namefmt, bool nodelivery);

#endif

