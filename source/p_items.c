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

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

enum {
   _imove_none,
   _imove_click,
   _imove_dragndrop,
};

static struct item *useitem, *selitem, *opnitem;
static i32 movitem;
static bool dragndrop;
static struct container inv[_inv_num];
static k32 selcx, selcy;

static void item_bbox(struct item *it, struct i32v4 *out, i32 ox, i32 oy) {
   if(it->container) {
      out->x = ox + it->container->x + it->x * 8;
      out->y = oy + it->container->y + it->y * 8;
      out->z = it->w * 8 - 1;
      out->w = it->h * 8 - 1;
   }
}

static void container_bbox(struct container *cont, struct i32v4 *out, i32 ox, i32 oy) {
   out->x = ox + cont->x;
   out->y = oy + cont->y;
   out->z = cont->w * 8;
   out->w = cont->h * 8;
}

static void ContainerInvTick(struct gui_state *g, struct container *cont) {
   if(!movitem && !dragndrop) {
      for_item(cont) {
         struct i32v4 bb;
         item_bbox(it, &bb, g->ox, g->oy);
         if(selitem != it && g->clicklft && aabb_point(bb.x, bb.y, bb.z, bb.w, g->cx, g->cy)) {
            selitem = it;
            AmbientSound(ss_player_cbi_invcur, 1.0);
            break;
         }
      }
   }
   for_item(cont) {
      if(it->InvTick) {
         it->InvTick(it, g);
      }
   }
}

static void ContainerShow(struct gui_state *g, struct container *cont) {
   struct i32v4 cbb;
   container_bbox(cont, &cbb, g->ox, g->oy);
   if(movitem &&
      (movitem == _imove_dragndrop ?
       g->old.clicklft && !g->clicklft :
       g->clicklft) &&
      aabb_point(cbb.x, cbb.y, cbb.z, cbb.w, g->cx, g->cy) &&
      (P_Inv_Place(cont, selitem, (g->cx - cbb.x) / 8, (g->cy - cbb.y) / 8) ||
       P_Inv_PlaceFirst(cont, selitem))) {
      movitem = _imove_none;
      dragndrop = false;
      AmbientSound(ss_player_cbi_invmov, 1.0);
   }
   str bg;
   switch(cont->type) {
   case _cont_store:  bg = sp_UI_InvBackStore;     break;
   case _cont_arms_u: bg = sp_UI_InvBackUpperArms; break;
   case _cont_arms_l: bg = sp_UI_InvBackLowerArms; break;
   case _cont_body:   bg = sp_UI_InvBackBody;      break;
   }
   PrintText_str(ns(lang(fast_strdup2(LANG "CONTAINER_", cont->cname))),
                 sf_smallfnt, g->defcr, cbb.x,1, cbb.y,2);
   for(i32 y = 0; y < cbb.w; y += 8) {
      for(i32 x = 0; x < cbb.z; x += 8) {
         PrintSprite(bg, cbb.x+x,1, cbb.y+y,1, _u_alpha, 0.8);
      }
   }
   for_item(cont) {
      if(movitem == _imove_dragndrop && selitem == it) {
         continue;
      }
      struct i32v4 bb;
      item_bbox(it, &bb, g->ox, g->oy);
      PrintSprite(it->spr, bb.x,1, bb.y,1);
      if(selitem == it) {
         i32 cr = movitem ? 0xF058AE : 0xF0E658;
         k32 a = (ACS_Sin((k32)ACS_Timer() / 105) + 1) / 2 / 3 + 0.33k;
         PrintRect(bb.x, bb.y, bb.z + 1, bb.w + 1, cr | (i32)(a * 255) << 24, _u_add);
      }
   }
   for_item(cont) {
      if(it->Show) {
         it->Show(it, g);
      }
   }
}

script static void BagItem_Tick(struct item *_item) {
   struct bagitem *item = (struct bagitem *)_item;
   for_item(&item->content) if(it->Tick) it->Tick(it);
}

script static void BagItem_InvTick(struct item *_item, struct gui_state *g) {
   struct bagitem *item = (struct bagitem *)_item;
   if(opnitem == _item) {
      ContainerInvTick(g, &item->content);
   }
}

script static void BagItem_Show(struct item *_item, struct gui_state *g) {
   struct bagitem *item = (struct bagitem *)_item;
   if(opnitem == _item) {
      ContainerShow(g, &item->content);
   }
}

script static void BagItem_Place(struct item *_item, struct container *cont) {
   struct bagitem *item = (struct bagitem *)_item;
   if(cont == &item->content) return;
   P_Item_Place(&item->item, cont);
}

script static void BagItem_Destroy(struct item *_item) {
   struct bagitem *item = (struct bagitem *)_item;
   ListDestroy(&item->content.head, {
     struct item *it = _obj;
     it->Destroy(it);
   });
   P_Item_Destroy(&item->item);
}

static bool ItemCanPlace(struct container *cont, struct item *item, i32 x, i32 y) {
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

static bool ItemCanPlaceAny(struct container *cont, struct item *item) {
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

void P_Inv_PInit(void) {
   static struct container const baseinv[] = {
      {       8*1, 64+8*-5, 11, 7, "Backpack", _cont_store},
      {294-48-8*8, 64+8*-2, 1,  3, "ArmUpL",   _cont_arms_u},
      {294-48-8*1, 64+8*-2, 1,  3, "ArmUpR",   _cont_arms_u},
      {294-48-8*9, 64+8* 3, 1,  3, "ArmLoL",   _cont_arms_l},
      {294-48-8*0, 64+8* 3, 1,  3, "ArmLoR",   _cont_arms_l},
      {294-48-8*6, 64+8* 5, 4,  1, "Belt",     _cont_store},
      {294-48-8*8, 64+8* 7, 1,  4, "LegL",     _cont_store},
      {294-48-8*1, 64+8* 7, 1,  4, "LegR",     _cont_store},
      {294-48-8*6, 64+8* 0, 4,  2, "Torso",    _cont_body},
      {294-48-8*5, 64+8*10, 2,  3, "Legs",     _cont_body},
   };
   fastmemcpy(inv, baseinv, sizeof baseinv);
   for(i32 i = 0; i < _inv_num; i++) {
      inv[i].head = nil;
   }
}

struct item *P_Item_New(struct item_info const *info) {
   struct item *item = Salloc(struct item, _tag_item);
   item->info    = *info;
   item->Destroy = P_Item_Destroy;
   item->Place   = P_Item_Place;
   return item;
}

script void P_Item_Destroy(struct item *item) {
   Dbg_Log(log_dev, _l("destroying item "), ACS_PrintHex((intptr_t)item));
   ServCallV(sm_DeleteItem, item);
   if(useitem == item) useitem = nil;
   if(opnitem == item) opnitem = nil;
   if(selitem == item) {
      selitem = nil;
      movitem = _imove_none;
      dragndrop = false;
   }
   P_Item_Unlink(item);
   Dalloc(item);
}

script bool P_Item_Use(struct item *item) {
   return ServCallI(sm_UseItem, item);
}

script void P_Item_Place(struct item *item, struct container *cont) {
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
                              struct item_info const *info) {
   struct bagitem *item = Salloc(struct bagitem, _tag_item);
   item->content.x     = 8;
   item->content.y     = 124;
   item->content.w     = w;
   item->content.h     = h;
   item->content.cname = "Item";
   item->content.type  = type;
   item->content.head  = nil;
   item->info    = *info;
   item->Tick    = BagItem_Tick;
   item->InvTick = BagItem_InvTick;
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

script bool P_Inv_PlaceFirst(struct container *cont, struct item *item) {
   for(i32 y = 0; y < cont->h; y++) {
      for(i32 x = 0; x < cont->w; x++) {
         if(P_Inv_Place(cont, item, x, y)) {
            return true;
         }
      }
   }
   return false;
}

script bool P_Inv_SwapFirst(struct container *cont, struct item *lhs) {
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
      if(P_Inv_PlaceFirst(&inv[i], item)) {
         return true;
      }
   }
   return false;
}

script void P_Inv_PTick(void) {
   if(useitem) {
      struct item *item = useitem;
      Dbg_Log(log_dev,
              _l("using "), _p(item->name), _c(' '), _c('('),
              ACS_PrintHex((intptr_t)item), _c(')'));
      if(item->Use && !item->Use(item)) {
         AmbientSound(ss_player_cbi_auto_invalid, 1.0);
      }
      useitem = nil;
   }
   for(i32 i = 0; i < _inv_num; i++) {
      for_item(&inv[i]) {
         if(it->Tick) {
            it->Tick(it);
         }
      }
   }
}

#define incY(amt) \
   statement({ \
      ny += amt; \
      nx  = inv[_inv_backpack].x; \
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
   PrintSprite(sp_UI_Body, g->ox+294-122,1, g->oy+24,1, _u_alpha, 0.6);
   PrintSprite(sp_UI_Bag,  g->ox+     16,1, g->oy+16,1, _u_alpha, 0.6);
   PrintText_str(sl_inv_hints, sf_smallfnt, g->defcr, g->ox+2,1, g->oy+212,2);
   for(i32 i = 0; i < _inv_num; i++) {
      ContainerInvTick(g, &inv[i]);
   }
   if(selitem) {
      i32 n = 0, x, y;
      i32 nx = inv[_inv_backpack].x;
      i32 ny = inv[_inv_backpack].y + 60;
      struct i32v4 bb;
      item_bbox(selitem, &bb, g->ox, g->oy);
      if(movitem == _imove_click || (!g->old.clicklft && !g->clicklft)) {
         if(movitem == _imove_dragndrop) {
            movitem = _imove_none;
         }
         dragndrop = false;
      }
      if(!movitem && !dragndrop && g->clicklft && !g->old.clicklft && aabb_point(bb.x, bb.y, bb.z, bb.w, g->cx, g->cy)) {
         selcx = g->cx;
         selcy = g->cy;
         dragndrop = true;
      }
      if(dragndrop && (fastabsk(selcx - g->cx) > 6 || fastabsk(selcy - g->cy) > 6)) {
         movitem = _imove_dragndrop;
      }
      if(g->clickrgt && !g->old.clickrgt) {
         movitem = movitem ? _imove_none : _imove_click;
         dragndrop = false;
      }
      setPos();
      PrintText_str(ns(lang(strp(_l(LANG "ITEM_TAG_"), _p(selitem->name)))),
                    sf_smallfnt, g->defcr, g->ox+x,1, g->oy+y,1);
      incY(8);
      setPos();
      PrintText_str(ns(lang(strp(_l(LANG "ITEM_SHORT_"), _p(selitem->name)))),
                    sf_smallfnt, g->defcr, g->ox+x,1, g->oy+y,1);
      incY(16);
      setPos();
      if(G_Button(g, tmpstr(sl_move), x, y,
                  .disabled = movitem == _imove_dragndrop,
                  .color = CR_LIGHTBLUE, Pre(btnclear))) {
         movitem = movitem ? _imove_none : _imove_click;
         dragndrop = false;
      }
      incPos();
      if(get_bit(selitem->flags, _if_equippable)) {
         setPos();
         if(G_Button(g, tmpstr(sl_equip), x, y,
                     .disabled = movitem == _imove_dragndrop,
                     .color = CR_LIGHTBLUE, Pre(btnclear))) {
            bool ok = false;
            for(i32 i = 0; i < _inv_num; i++) {
               struct container *cont = &inv[i];
               if(cont->type == selitem->equip &&
                  (P_Inv_PlaceFirst(cont, selitem) ||
                   P_Inv_SwapFirst(cont, selitem))) {
                  ok = true;
                  break;
               }
            }
            AmbientSound(ok ? ss_player_cbi_invmov : ss_player_cbi_auto_invalid, 1.0);
         }
         incPos();
      }
      if(get_bit(selitem->flags, _if_openable)) {
         setPos();
         if(G_Button(g, tmpstr(opnitem == selitem ? sl_close : sl_open),
                     x, y, .disabled = movitem == _imove_dragndrop,
                     .color = CR_LIGHTBLUE, Pre(btnclear))) {
            opnitem = opnitem != selitem ? selitem : nil;
         }
         incPos();
      }
      if(selitem->Use) {
         setPos();
         if(G_Button(g, tmpstr(sl_use), x, y,
                     .disabled = movitem == _imove_dragndrop,
                     .color = CR_RED, Pre(btnclear))) {
            useitem = selitem;
         }
         incPos();
      }
      setPos();
      if(selitem->scr > 0) {
         ACS_BeginPrint();
         ACS_PrintChar('(');
         PrintStr(scoresep(selitem->scr));
         PrintStrL("\Cnscr\C-)");
         PrintText(sf_smallfnt, g->defcr, g->ox+x+18,1, g->oy+y,1);
      }
      static struct gui_fil itemfill = {26};
      if(selitem->scr >= 0 &&
         G_Button(g, tmpstr(selitem->scr > 0 ? sl_sell : sl_discard),
                  x, y, .disabled = movitem == _imove_dragndrop,
                  .color = CR_RED, .fill = &itemfill,
                  Pre(btnclear))) {
         if(selitem->scr) {
            P_Scr_GivePos(x, y, selitem->scr, true);
         }
         selitem->Destroy(selitem);
         AmbientSound(ss_player_cbi_invrem, 1.0);
         incPos();
      }
   }
   for(i32 i = 0; i < _inv_num; i++) {
      ContainerShow(g, &inv[i]);
   }
   if(movitem == _imove_dragndrop && selitem) {
      PrintSprite(selitem->spr, g->cx,1, g->cy,1, _u_alpha, 0.8k);
   }
}

static struct item_info ItemInfo(void) {
   struct item_info info = {
      .w     = GetMembI(0, sm_W),
      .h     = GetMembI(0, sm_H),
      .equip = GetMembI(0, sm_InvEquip),
      .scr   = GetMembI(0, sm_InvSell),
      .name  = GetMembS(0, sm_InvName),
   };
   info.spr = strp(_l(":Items:"), _p(info.name));
   if(info.equip != _cont_store) {
      set_bit(info.flags, _if_equippable);
   }
   return info;
}

script_str ext("ACS") addr(OBJ "ItemCreate") struct item *Z_ItemCreate(void) {
   struct item_info info = ItemInfo();
   Dbg_Log(log_dev, _l("creating "), _p(info.name));
   i32 itemt = GetMembI(0, sm_ItemType);
   switch(itemt) {
   case _itemt_item:
      return P_Item_New(&info);
   case _itemt_bagitem:
      set_bit(info.flags, _if_openable);
      return &P_BagItem_New(
         GetMembI(0, sm_InvW),
         GetMembI(0, sm_InvH),
         GetMembI(0, sm_InvType),
         &info)->item;
   case _itemt_useitem:
      struct item *item = P_Item_New(&info);
      item->Use = P_Item_Use;
      return item;
   default:
      PrintErr(_l("unknown item type "), _p(itemt));
   }
   return nil;
}

void P_ItemPopup(str tag, k32 x, k32 y, k32 z) {
   bool seen;
   struct i32v2 vp = project(x, y, z, &seen);
   if(!seen) {
      vp.x = 320/2 + ACS_Random(-64, 64);
      vp.y = 240/2 + ACS_Random(-48, 48);
   }
   DrawCallV(sm_AddItemPop, vp.x, vp.y, tag);
}

script_str ext("ACS") addr(OBJ "ItemPopupAmmo") void Z_ItemPopupAmmo(k32 x, k32 y, k32 z) {
   i32 ammodisp = CVarGetI(sc_player_ammodisp);
   if(ammodisp & _itm_disp_log) {
      P_LogH(1, "%S", GetMembS(0, sm_LogText));
   }
   if(ammodisp & _itm_disp_pop) {
      P_ItemPopup(GetMembS(0, sm_AmmoText), x, y, z);
   }
}

script_str ext("ACS") addr(OBJ "ItemPopupTag") void Z_ItemPopupTag(void) {
   if(CVarGetI(sc_player_itemdisp) & _itm_disp_pop) {
      P_ItemPopup(GetNameTag(0), GetX(0), GetY(0), GetZ(0));
   }
}

script_str ext("ACS") addr(OBJ "ItemAttach") bool Z_ItemAttach(struct item *item) {
   Dbg_Log(log_dev, _l("attaching item "), ACS_PrintHex((intptr_t)item));
   if(P_Inv_Add(item)) {
      Z_ItemPopupTag();
      return true;
   } else {
      return false;
   }
}

script_str ext("ACS") addr(OBJ "ItemDetach") void Z_ItemDetach(struct item *item) {
   if(item->Destroy) {
      Dbg_Log(log_dev, _l("detaching item "), ACS_PrintHex((intptr_t)item));
      item->Destroy(item);
   }
}

script_str ext("ACS") addr(OBJ "ItemCanPlace") bool Z_ItemCanPlace(struct item *item) {
   for(i32 i = 0; i < _inv_num; i++) {
      if(ItemCanPlaceAny(&inv[i], item)) {
         return true;
      }
   }
   return false;
}

script_str ext("ACS") addr(OBJ "ItemOnBody") bool Z_ItemOnBody(struct item *item) {
   return item->container && item->container->type == _cont_body;
}

/* EOF */
