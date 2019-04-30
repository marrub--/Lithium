/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Inventory items.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef items_h
#define items_h

#define for_item(cont) for_list(struct item *it, (cont).items) if(it)

struct itemdata
{
   str name, spr, tag;
   u32 w, h;
   i96 scr;

   /* these need to be ScriptS or they'll disappear on map load */
   /* (we shouldn't use a MInit callback because it would cause too much recursion) */
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

optargs(1) void P_Item_Init(struct item *item, struct itemdata const *data);
optargs(1) struct item *P_Item_New(struct itemdata const *data);
void P_Item_Unlink(struct item *item);

script void P_Item_Destroy(struct item *item);
script bool P_Item_Use(struct item *item);
script void P_Item_Place(struct item *item, struct container *cont);

optargs(1) struct bagitem *P_BagItem_New(i32 w, i32 h, str bg, struct itemdata const *data);

void P_Inv_PInit(struct player *p);
void P_Inv_PQuit(struct player *p);

bool P_Inv_Place(struct container *cont, struct item *item, i32 x, i32 y);
script bool P_Inv_PlaceFirst(struct container *cont, struct item *item);
bool P_Inv_Add(struct player *p, struct item *item);

#endif
