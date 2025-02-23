// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Inventory items.                                                         │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef items_h
#define items_h

enum ZscName(Container) {
   _cont_store,
   _cont_arms_u,
   _cont_arms_l,
   _cont_body,
};

enum ZscName(InventoryItemType) {
   _itemt_item,
   _itemt_bagitem,
   _itemt_useitem,
};

#if !ZscOn
#define for_item(cont) for_list(struct item *it, (cont)->head)

enum {
   _if_equippable,
   _if_openable,
};

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

script funcdef bool (*item_use_t    )(struct item *item);
script funcdef void (*item_tick_t   )(struct item *item);
script funcdef void (*item_invtick_t)(struct item *item, struct gui_state *g);
script funcdef void (*item_show_t   )(struct item *item, struct gui_state *g);
script funcdef void (*item_destroy_t)(struct item *item);
script funcdef void (*item_place_t  )(struct item *item, struct container *cont);

struct item_info {
   str     name, spr;
   i32     w, h;
   i32     equip;
   score_t scr;
   i32     flags;
   item_use_t     Use;
   item_tick_t    Tick;
   item_invtick_t InvTick;
   item_show_t    Show;
   item_destroy_t Destroy;
   item_place_t   Place;
};

struct item {
   anonymous struct item_info info;
   i32 x, y;
   struct container *container;
   struct list link;
};

struct container {
   i32          x, y, w, h;
   cstr         cname;
   i32          type;
   struct list *head;
};

struct bagitem {
   anonymous struct item item;
   struct container content;
};

void P_Inv_PInit(void);

optargs(1) struct item *P_Item_New(struct item_info const *info);
void P_Item_Unlink(struct item *item);
script void P_Item_Destroy(struct item *item);
script bool P_Item_Use(struct item *item);
script void P_Item_Place(struct item *item, struct container *cont);

optargs(1) struct bagitem *P_BagItem_New(i32 w, i32 h, i32 type, struct item_info const *info);

bool P_Inv_Place(struct container *cont, struct item *item, i32 x, i32 y);
script bool P_Inv_PlaceFirst(struct container *cont, struct item *item);
script bool P_Inv_SwapFirst(struct container *cont, struct item *lhs);
bool P_Inv_Swap(struct item *lhs, struct item *rhs);
bool P_Inv_Add(struct item *item);

void P_ItemPopup(str tag, k32 x, k32 y, k32 z);
#endif

#endif
