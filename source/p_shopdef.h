// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Unified shop definitions.                                                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef p_shopdef_h
#define p_shopdef_h

enum {
   _shop_items,
   _shop_upgrades,
};

struct shopdef {
   i32     shoptype;
   cstr    name;
   cstr    bipunlock;
   score_t cost;
};

struct shopitem {
   anonymous struct shopdef shopdef;
   i32 pclass;
   i32 count;
   str classname;
   bool weapon;
};

bool P_Shop_CanBuy(struct shopdef *def);
optargs(1) bool P_Shop_Buy(struct shopdef *def, cstr name, bool nodelivery, bool nolog);

bool Shop_CanBuy(struct shopitem const *item);
void Shop_Buy(struct shopitem const *item);
bool Shop_Give(struct shopitem const *item, i32 tid);

#endif
