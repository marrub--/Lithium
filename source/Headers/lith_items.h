// Copyright © 2017 Graham Sanderson, all rights reserved.
#ifndef LITH_ITEMS_H
#define LITH_ITEMS_H

#define foritem(cont) forlist(item_t *it, (cont).items) if(it)

// NB: these need to be ScriptS or they'll disappear on map load
[[__call("ScriptS")]] typedef bool (*item_b_cb_t)(struct item *item);
[[__call("ScriptS")]] typedef void (*item_v_cb_t)(struct item *item);

typedef struct itemdata
{
   int w, h;
   __str spr;

   item_b_cb_t Use;
   item_v_cb_t Tick;
   item_v_cb_t Destroy;
} itemdata_t;

typedef struct item
{
   [[__anonymous]] itemdata_t data;

   int x, y;

   struct container *container;
   list_t link;
} item_t;

typedef struct container
{
   int w, h;
   __str bg;
   list_t items;
   struct player *user;
} container_t;

typedef struct bagitem
{
   [[__anonymous]] item_t item;
   container_t content;
} bagitem_t;

[[__optional_args(1)]] item_t *Lith_Item_New(itemdata_t const *data);
[[__call("ScriptS")]] void Lith_Item_Destroy(item_t *item);
bagitem_t *Lith_BagItem_New(int w, int h, __str bg);
bool Lith_ItemPlace(container_t *cont, item_t *item, int x, int y);

#endif
