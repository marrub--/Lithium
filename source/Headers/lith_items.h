// Copyright © 2017-2018 Alison Sanderson, all rights reserved.
#ifndef lith_items_h
#define lith_items_h

#define foritem(cont) forlist(item_t *it, (cont).items) if(it)

// NB: these need to be ScriptS or they'll disappear on map load
//     (we can't use a GSReinit callback because that would be really bad)
script typedef bool (*item_b_cb_t)(struct item *item);
script typedef void (*item_v_cb_t)(struct item *item);
script typedef void (*item_p_cb_t)(struct item *item, struct container *cont);

typedef struct itemdata
{
   __str name, spr, tag;
   uint w, h;
   i96 scr;

   item_b_cb_t Use;
   item_v_cb_t Tick;
   item_v_cb_t Destroy;
   item_p_cb_t Place;
} itemdata_t;

typedef struct item
{
   anonymous itemdata_t data;

   uint x, y;

   struct container *container;
   struct player *user;

   list_t link;
} item_t;

typedef struct container
{
   uint w, h;
   __str bg;
   list_t items;
   struct player *user;
} container_t;

typedef struct bagitem
{
   anonymous item_t item;
   container_t content;
} bagitem_t;

void Lith_PlayerInitInventory(struct player *p);
void Lith_PlayerDeallocInventory(struct player *p);

optargs(1) void Lith_Item_Init(item_t *item, itemdata_t const *data);
optargs(1) item_t *Lith_Item_New(itemdata_t const *data);
void Lith_Item_Unlink(item_t *item);

script void Lith_Item_Destroy(item_t *item);
script bool Lith_Item_Use(item_t *item);
script void Lith_Item_Place(item_t *item, container_t *cont);

optargs(1) bagitem_t *Lith_BagItem_New(int w, int h, __str bg, itemdata_t const *data);

bool Lith_ItemPlace(container_t *cont, item_t *item, int x, int y);

script bool Lith_ItemPlaceFirst(container_t *cont, item_t *item);

bool Lith_PlayerAddItem(struct player *p, item_t *item);

#endif
