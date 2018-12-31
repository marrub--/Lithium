// Copyright © 2017-2018 Alison Sanderson, all rights reserved.
#ifndef items_h
#define items_h

#define for_item(cont) for_list(struct item *it, (cont).items) if(it)

struct itemdata
{
   str name, spr, tag;
   u32 w, h;
   i96 scr;

   // these need to be ScriptS or they'll disappear on map load
   // (we can't use a GSReinit callback because that would be really bad)
   script bool (*Use)(struct item *);
   script void (*Tick)(struct item *);
   script void (*Destroy)(struct item *);
   script void (*Place)(struct item *, struct container *);
};

struct item
{
   anonymous struct itemdata data;

   u32 x, y;

   struct container *container;
   struct player *user;

   list link;
};

struct container
{
   u32  w, h;
   str  bg;
   list items;
   struct player *user;
};

struct bagitem
{
   anonymous struct item item;
   struct container content;
};

void Lith_PlayerInitInventory(struct player *p);
void Lith_PlayerDeallocInventory(struct player *p);

optargs(1) void Lith_Item_Init(struct item *item, struct itemdata const *data);
optargs(1) struct item *Lith_Item_New(struct itemdata const *data);
void Lith_Item_Unlink(struct item *item);

script void Lith_Item_Destroy(struct item *item);
script bool Lith_Item_Use(struct item *item);
script void Lith_Item_Place(struct item *item, struct container *cont);

optargs(1) struct bagitem *Lith_BagItem_New(i32 w, i32 h, str bg, struct itemdata const *data);

bool Lith_ItemPlace(struct container *cont, struct item *item, i32 x, i32 y);

script bool Lith_ItemPlaceFirst(struct container *cont, struct item *item);

bool Lith_PlayerAddItem(struct player *p, struct item *item);

#endif
