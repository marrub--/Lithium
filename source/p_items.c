// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Inventory handling and UI.                                               │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "w_world.h"

static
void Container(struct gui_state *g, struct container *cont, i32 sx, i32 sy) {
   sx += g->ox;
   sy += g->oy;

   str bg;
   switch(cont->type) {
      case _cont_store:  bg = sp_UI_InvBackStore;     break;
      case _cont_arms_u: bg = sp_UI_InvBackUpperArms; break;
      case _cont_arms_l: bg = sp_UI_InvBackLowerArms; break;
      case _cont_body:   bg = sp_UI_InvBackBody;      break;
   }

   i32 h = cont->h * 8;
   i32 w = cont->w * 8;

   PrintText_str(ns(lang_fmt(LANG "CONTAINER_%s", cont->cname)),
                 sf_smallfnt, g->defcr, sx,1, sy,2);

   for(i32 y = 0; y < h; y += 8) {
      for(i32 x = 0; x < w; x += 8) {
         PrintSpriteA(bg, sx+x,1, sy+y,1, 0.8);
      }
   }

   if(pl.movitem && g->clicklft &&
      aabb_point(sx, sy, w, h, g->cx, g->cy) &&
      (P_Inv_Place(cont, pl.selitem, (g->cx - sx) / 8, (g->cy - sy) / 8) ||
       P_Inv_PlaceFirst(cont, pl.selitem))) {
      pl.movitem = false;
      AmbientSound(ss_player_cbi_invmov, 1.0);
   }

   bool didresel = false; /* Basic bitch hack. -Ten */

   for_item(cont) {
      i32 x = sx + it->x * 8;
      i32 y = sy + it->y * 8;
      i32 ex = it->w * 8 - 1;
      i32 ey = it->h * 8 - 1;

      PrintSprite(it->spr, x,1, y,1);

      if(pl.movitem) {
         continue;
      }

      if(!didresel && pl.selitem != it && g->clicklft && aabb_point(x, y, ex, ey, g->cx, g->cy)) {
         didresel = true;
         pl.selitem = it;
         AmbientSound(ss_player_cbi_invcur, 1.0);
      }

      if(pl.selitem == it) {
         k32 a = (ACS_Sin(ACS_Timer() / 105.0k) * 0.5k + 1.2k) / 4.0k;
         PrintRect(x, y, ex + 1, ey + 1, 0xF0E658 | (i32)(a * 255.0k) << 24);
      }
   }
}

script static
void BagItem_Tick(struct item *_item) {
   struct bagitem *item = (struct bagitem *)_item;
   for_item(&item->content) if(it->Tick) it->Tick(it);
}

script static
void BagItem_Show(struct item *_item, struct gui_state *g, i32 y) {
   struct bagitem *item = (struct bagitem *)_item;
   Container(g, &item->content, 8, y);
}

script static
void BagItem_Place(struct item *_item, struct container *cont) {
   struct bagitem *item = (struct bagitem *)_item;
   if(cont == &item->content) return;
   P_Item_Place(&item->item, cont);
}

script static
void BagItem_Destroy(struct item *_item) {
   struct bagitem *item = (struct bagitem *)_item;
   ListDestroy(&item->content.head, {
     struct item *it = _obj;
     it->Destroy(it);
   });
   P_Item_Destroy(&item->item);
}

static
bool ItemCanPlace(struct container *cont, struct item *item, i32 x, i32 y) {
   if(x < 0 || y < 0 ||
      x + item->w > cont->w || y + item->h > cont->h) {
      return false;
   }

   for_item(cont) if(it != item) {
      /* there is certainly a better way of doing this that still
       * accounts for overlap but,
       * damn do i not care -fira
       */
      for(i32 ox = it->x; ox < it->x + it->w; ox++) {
         for(i32 oy = it->y; oy < it->y + it->h; oy++) {
            for(i32 ix = x; ix < x + item->w; ix++) {
               for(i32 iy = y; iy < y + item->h; iy++) {
                  if(ox == ix && oy == iy) {
                     return false;
                  }
               }
            }
         }
      }
   }

   return true;
}

static
bool ItemCanPlaceAny(struct container *cont, struct item *item) {
   i32 xn = cont->w / item->w;
   i32 yn = cont->h / item->h;

   for(i32 y = 0; y < yn; y++) {
      for(i32 x = 0; x < xn; x++) {
         if(ItemCanPlace(cont, item, x * item->w, y * item->h)) {
            return true;
         }
      }
   }

   return false;
}

static
void EquipItem(struct item *sel) {
   bool ok = false;

   for(i32 i = 0; i < _inv_num; i++) {
      struct container *cont = &pl.inv[i];

      if(cont->type == sel->equip && (P_Inv_PlaceFirst(cont, sel) ||
                                      P_Inv_SwapFirst(cont, sel))) {
         ok = true;
         break;
      }
   }

   AmbientSound(ok ? ss_player_cbi_invmov : ss_player_cbi_auto_invalid, 1.0);
}

void P_Inv_PInit() {
   static
   struct container const baseinv[] = {
      [_inv_backpack]    = {11, 7, "Backpack", _cont_store},
      [_inv_arm_upper_l] = {1,  3, "ArmUpL",   _cont_arms_u},
      [_inv_arm_upper_r] = {1,  3, "ArmUpR",   _cont_arms_u},
      [_inv_arm_lower_l] = {1,  3, "ArmLoL",   _cont_arms_l},
      [_inv_arm_lower_r] = {1,  3, "ArmLoR",   _cont_arms_l},
      [_inv_belt]        = {4,  1, "Belt",     _cont_store},
      [_inv_leg_l]       = {1,  4, "LegL",     _cont_store},
      [_inv_leg_r]       = {1,  4, "LegR",     _cont_store},
      [_inv_torso]       = {4,  2, "Torso",    _cont_body},
      [_inv_legs]        = {2,  3, "Legs",     _cont_body},
   };

   fastmemcpy(pl.inv, baseinv, sizeof baseinv);

   for(i32 i = 0; i < _inv_num; i++) {
      pl.inv[i].head = nil;
   }

   pl.invinit = true;
}

void P_Inv_PQuit() {
   for(i32 i = 0; i < _inv_num; i++) {
      ListDestroy(&pl.inv[i].head, {
         struct item *it = _obj;
         it->Destroy(it);
      });
   }

   pl.useitem = pl.selitem = pl.opnitem = nil;
   pl.movitem = false;

   pl.invinit = false;
}

struct item *P_Item_New(struct itemdata const *data) {
   struct item *item = Salloc(struct item, _tag_item);

   item->data    = *data;
   item->Destroy = P_Item_Destroy;
   item->Place   = P_Item_Place;

   return item;
}

script
void P_Item_Destroy(struct item *item) {
   Dbg_Log(log_dev, "P_Item_Destroy: destroying item %p", item);

   ServCallI(sm_DeleteItem, item);

   if(!P_None()) {
      if(pl.useitem == item) pl.useitem = nil;
      if(pl.selitem == item) pl.selitem = nil;
      if(pl.opnitem == item) pl.opnitem = nil;
      pl.movitem = false;
   }

   P_Item_Unlink(item);
   Dalloc(item);
}

script
bool P_Item_Use(struct item *item) {
   return ServCallI(sm_UseItem, item);
}

script
void P_Item_Place(struct item *item, struct container *cont) {
   P_Item_Unlink(item);
   ListInsert(&cont->head, &item->link, item);
   item->container = cont;
}

void P_Item_Unlink(struct item *item) {
   ListRemove(&item->link);
   if(item->container) {
      item->container = nil;
   }
}

struct bagitem *P_BagItem_New(i32 w, i32 h, i32 type,
                              struct itemdata const *data) {
   struct bagitem *item = Salloc(struct bagitem, _tag_item);

   item->content.w     = w;
   item->content.h     = h;
   item->content.cname = "Item";
   item->content.type  = type;
   item->content.head  = nil;

   item->data    = *data;
   item->Tick    = BagItem_Tick;
   item->Show    = BagItem_Show;
   item->Destroy = BagItem_Destroy;
   item->Place   = BagItem_Place;

   return item;
}

bool P_Inv_Place(struct container *cont, struct item *item, i32 x, i32 y) {
   if(!ItemCanPlace(cont, item, x, y)) {
      return false;
   }

   item->Place(item, cont);

   item->x = x;
   item->y = y;

   return true;
}

script
bool P_Inv_PlaceFirst(struct container *cont, struct item *item) {
   for(i32 y = 0; y < cont->h; y++) {
      for(i32 x = 0; x < cont->w; x++) {
         if(P_Inv_Place(cont, item, x, y)) {
            return true;
         }
      }
   }

   return false;
}

script
bool P_Inv_SwapFirst(struct container *cont, struct item *lhs) {
   for_item(cont) {
      if(P_Inv_Swap(lhs, it)) {
         return true;
      }
   }
   return false;
}

bool P_Inv_Swap(struct item *lhs, struct item *rhs) {
   /* naive way of doing it, but for our purposes no problem, just swap
    * only if there is no difference in size.
    */
   if(lhs->w == rhs->w && lhs->h == rhs->h) {
      struct container *lhs_cont = lhs->container;
      struct container *rhs_cont = rhs->container;
      swap(i32, lhs->x, rhs->x);
      swap(i32, lhs->y, rhs->y);
      lhs->Place(lhs, rhs_cont);
      rhs->Place(rhs, lhs_cont);
      return true;
   } else {
      return false;
   }
}

bool P_Inv_Add(struct item *item) {
   for(i32 i = 0; i < _inv_num; i++) {
      if(P_Inv_PlaceFirst(&pl.inv[i], item)) {
         return true;
      }
   }

   return false;
}

script
void P_Inv_PTick() {
   if(pl.useitem) {
      struct item *item = pl.useitem;

      Dbg_Log(log_dev, "using %S (%p)", item->name, item);
      if(item->Use && !item->Use(item)) {
         AmbientSound(ss_player_cbi_auto_invalid, 1.0);
      }

      pl.useitem = nil;
   }

   for(i32 i = 0; i < _inv_num; i++) {
      for_item(&pl.inv[i]) {
         if(it->Tick) {
            it->Tick(it);
         }
      }
   }
}

#define incY(amt) \
   statement({ \
      ny += amt; \
      nx  = xs[_inv_backpack]; \
      n   = 0; \
   })
#define incPos() \
   statement({ \
      if(++n == 3) incY(8); \
      else         nx += 32; \
   })
#define setPos() \
   statement({ \
      x = nx; \
      y = ny; \
   })

void P_CBI_TabItems(struct gui_state *g) {
   static
   i32 const xs[] = {
      [_inv_backpack]    =        8*1,
      [_inv_arm_upper_l] = 294-48-8*8,
      [_inv_arm_upper_r] = 294-48-8*1,
      [_inv_arm_lower_l] = 294-48-8*9,
      [_inv_arm_lower_r] = 294-48-8*0,
      [_inv_belt]        = 294-48-8*6,
      [_inv_leg_l]       = 294-48-8*8,
      [_inv_leg_r]       = 294-48-8*1,
      [_inv_torso]       = 294-48-8*6,
      [_inv_legs]        = 294-48-8*5,
   };

   static
   i32 const ys[] = {
      [_inv_backpack]    = 64+8*-5,
      [_inv_arm_upper_l] = 64+8*-2,
      [_inv_arm_upper_r] = 64+8*-2,
      [_inv_arm_lower_l] = 64+8* 3,
      [_inv_arm_lower_r] = 64+8* 3,
      [_inv_belt]        = 64+8* 5,
      [_inv_leg_l]       = 64+8* 7,
      [_inv_leg_r]       = 64+8* 7,
      [_inv_torso]       = 64+8* 0,
      [_inv_legs]        = 64+8*10,
   };

   PrintSpriteA(sp_UI_Body, g->ox+294-122,1, g->oy+24,1, 0.6);
   PrintSpriteA(sp_UI_Bag,  g->ox+     16,1, g->oy+16,1, 0.6);

   PrintText_str(ns(lang(sl_inv_hints)),
                 sf_smallfnt, g->defcr, g->ox+2,1, g->oy+212,2);

   for(i32 i = 0; i < _inv_num; i++) {
      Container(g, &pl.inv[i], xs[i], ys[i]);
   }

   struct item *sel = pl.selitem;

   if(sel) {
      i32 n = 0, x, y;
      i32 nx = xs[_inv_backpack];
      i32 ny = ys[_inv_backpack] + 60;

      if(g->clickrgt && !g->old.clickrgt) {
         pl.movitem = !pl.movitem;
      }

      setPos();
      PrintText_str(ns(lang_fmt(LANG "ITEM_TAG_%S", sel->name)),
                    sf_smallfnt, g->defcr, g->ox+x,1, g->oy+y,1);
      incY(8);

      setPos();
      PrintText_str(ns(lang_fmt(LANG "ITEM_SHORT_%S", sel->name)),
                    sf_smallfnt, g->defcr, g->ox+x,1, g->oy+y,1);
      incY(16);

      setPos();
      if(G_Button(g, tmpstr(lang(sl_move)), x, y, .color = "n", Pre(btnclear))) {
         pl.movitem = !pl.movitem;
      }
      incPos();

      if(get_bit(sel->flags, _if_equippable)) {
         setPos();
         if(G_Button(g, tmpstr(lang(sl_equip)), x, y, .color = "n",
                     Pre(btnclear))) {
            EquipItem(sel);
         }
         incPos();
      }

      if(get_bit(sel->flags, _if_openable)) {
         setPos();
         if(G_Button(g, tmpstr(pl.opnitem == sel ?
                               lang(sl_close) :
                               lang(sl_open)),
                     x, y, .color = "n", Pre(btnclear)))
         {
            pl.opnitem = pl.opnitem != sel ? sel : nil;
         }
         incPos();
      }

      if(sel->Use) {
         setPos();
         if(G_Button(g, tmpstr(lang(sl_use)), x, y, .color = "g", Pre(btnclear))) {
            pl.useitem = sel;
         }
         incPos();
      }

      setPos();
      if(sel->scr > 0) {
         PrintTextFmt("(%s\Cnscr\C-)", scoresep(sel->scr));
         PrintText(sf_smallfnt, g->defcr, g->ox+x+18,1, g->oy+y,1);
      }

      if(sel->scr >= 0 &&
         G_Button(g, tmpstr(sel->scr > 0 ? lang(sl_sell) : lang(sl_discard)),
                  x, y, .color = "g", .fill = &CBIState(g)->itemfill,
                  Pre(btnclear))) {
         if(sel->scr) {
            P_Scr_GivePos(x, y, sel->scr, true);
         }
         sel->Destroy(sel);
         AmbientSound(ss_player_cbi_invrem, 1.0);
         incPos();
      }

      ny = y;
      incY(16);
      setPos();
      if(pl.opnitem && pl.opnitem->Show) {
         pl.opnitem->Show(pl.opnitem, g, y);
      }
   }
}

static
struct itemdata ItemData() {
   struct itemdata data = {
      .w     = GetMembI(0, sm_W),
      .h     = GetMembI(0, sm_H),
      .equip = GetMembI(0, sm_InvEquip),
      .scr   = GetMembI(0, sm_InvSell),
      .name  = GetMembS(0, sm_InvName),
   };
   data.spr = ns(lang_fmt(LANG "Item_%S", data.name));
   if(data.equip != _cont_store) set_bit(data.flags, _if_equippable);
   return data;
}

script_str ext("ACS") addr(OBJ "ItemCreate")
struct item *Sc_ItemCreate() {
   struct itemdata data = ItemData();
   Dbg_Log(log_dev, "%s: creating %S", __func__, data.name);
   return P_Item_New(&data);
}

script_str ext("ACS") addr(OBJ "BagItemCreate")
struct item *Sc_BagItemCreate() {
   struct itemdata data = ItemData();
   set_bit(data.flags, _if_openable);
   Dbg_Log(log_dev, "%s: creating %S", __func__, data.name);
   return &P_BagItem_New(
      GetMembI(0, sm_InvW),
      GetMembI(0, sm_InvH),
      GetMembI(0, sm_InvType),
      &data
   )->item;
}

void P_ItemPopup(str tag, k32 x, k32 y, k32 z) {
   bool seen;
   struct i32v2 vp = project(x, y, z, &seen);
   if(!seen) {
      vp.x = 320/2 + ACS_Random(-64, 64);
      vp.y = 240/2 + ACS_Random(-48, 48);
   }
   DrawCallI(sm_AddItemPop, vp.x, vp.y, tag);
}

script_str ext("ACS") addr(OBJ "ItemPopupAmmo")
void Sc_ItemPopupAmmo(k32 x, k32 y, k32 z) {
   i32 ammodisp = CVarGetI(sc_player_ammodisp);
   if(ammodisp & _itm_disp_log) {
      pl.logH(1, "%S", GetMembS(0, sm_LogText));
   }
   if(ammodisp & _itm_disp_pop) {
      P_ItemPopup(GetMembS(0, sm_AmmoText), x, y, z);
   }
}

script_str ext("ACS") addr(OBJ "ItemPopupTag")
void Sc_ItemPopupTag(void) {
   if(CVarGetI(sc_player_itemdisp) & _itm_disp_pop) {
      P_ItemPopup(GetNameTag(0), GetX(0), GetY(0), GetZ(0));
   }
}

script_str ext("ACS") addr(OBJ "ItemAttach")
bool Sc_ItemAttach(struct item *item) {
   Dbg_Log(log_dev, "%s: attaching item %p", __func__, item);

   if(!P_None()) {
      if(P_Inv_Add(item)) {
         Sc_ItemPopupTag();
         return true;
      } else {
         return false;
      }
   }

   return false;
}

script_str ext("ACS") addr(OBJ "ItemDetach")
void Sc_ItemDetach(struct item *item) {
   if(item->Destroy) {
      Dbg_Log(log_dev, "%s: detaching item %p", __func__, item);
      item->Destroy(item);
   }
}

script_str ext("ACS") addr(OBJ "ItemCanPlace")
bool Sc_ItemCanPlace(struct item *item) {
   if(!P_None()) {
      for(i32 i = 0; i < _inv_num; i++) {
         if(ItemCanPlaceAny(&pl.inv[i], item)) {
            return true;
         }
      }
   }

   return false;
}

script_str ext("ACS") addr(OBJ "ItemOnBody")
bool Sc_ItemOnBody(struct item *item) {
   return item->container && item->container->type == _cont_body;
}

/* EOF */
