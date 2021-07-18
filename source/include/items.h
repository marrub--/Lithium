/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
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

enum ZscName(Container) {
   _cont_store,
   _cont_arms_u,
   _cont_arms_l,
   _cont_body,
};

#if !ZscOn
#define for_item(cont) for_list(struct item *it, (cont)->head)

enum {
   _inv_backpack,
   _inv_arm_upper_l,
   _inv_arm_upper_r,
   _inv_arm_lower_l,
   _inv_arm_lower_r,
   _inv_belt,
   _inv_leg_l,
   _inv_leg_r,
   _inv_torso,
   _inv_legs,
   _inv_num,
};

enum {
   _if_equippable,
   _if_openable,
};

script funcdef bool (*item_use_t    )(struct item *item);
script funcdef void (*item_tick_t   )(struct item *item);
script funcdef void (*item_show_t   )(struct item *item, struct gui_state *g, i32 y);
script funcdef void (*item_destroy_t)(struct item *item);
script funcdef void (*item_place_t  )(struct item *item, struct container *cont);

struct itemdata {
   str name, spr;
   u32 w, h;
   u32 equip;
   i96 scr;
   u32 flags;

   item_use_t     Use;
   item_tick_t    Tick;
   item_show_t    Show;
   item_destroy_t Destroy;
   item_place_t   Place;
};

struct item {
   anonymous struct itemdata data;

   u32 x, y;

   struct container *container;

   struct list link;
};

struct container {
   u32          w, h;
   cstr         cname;
   i32          type;
   struct list *head;
};

struct bagitem {
   anonymous struct item item;
   struct container content;
};

optargs(1) struct item *P_Item_New(struct itemdata const *data);
void P_Item_Unlink(struct item *item);

script void P_Item_Destroy(struct item *item);
script bool P_Item_Use(struct item *item);
script void P_Item_Place(struct item *item, struct container *cont);

optargs(1) struct bagitem *P_BagItem_New(i32 w, i32 h, i32 type, struct itemdata const *data);

void P_Inv_PInit(void);
void P_Inv_PQuit(void);

bool P_Inv_Place(struct container *cont, struct item *item, i32 x, i32 y);
script bool P_Inv_PlaceFirst(struct container *cont, struct item *item);
script bool P_Inv_SwapFirst(struct container *cont, struct item *lhs);
bool P_Inv_Swap(struct item *lhs, struct item *rhs);
bool P_Inv_Add(struct item *item);

void P_ItemPopup(str tag, k32 x, k32 y, k32 z);
#endif

#endif
