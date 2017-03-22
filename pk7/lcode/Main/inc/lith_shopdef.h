#ifndef LITH_SHOPDEF_H
#define LITH_SHOPDEF_H

enum
{
   shoptype_items,
   shoptype_upgrades,
   shoptype_max
};

typedef struct shopdef_s
{
   __str name;
   __str bipunlock;
   score_t cost;
   void (*shopBuy)   (struct player_s *p, struct shopdef_s const *def, void *obj);
   bool (*shopCanBuy)(struct player_s *p, struct shopdef_s const *def, void *obj);
   void (*shopGive)  (int tid,            struct shopdef_s const *def, void *obj);
} shopdef_t;

score_t Lith_ShopGetCost(struct player_s *p, shopdef_t const *def);
bool Lith_ShopCanBuy    (struct player_s *p, shopdef_t const *def, void *obj);
bool Lith_ShopBuy       (struct player_s *p, shopdef_t const *def, void *obj, __str namefmt, bool nodelivery);

#endif

