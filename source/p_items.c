/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Inventory handling and UI.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"

/* Static Functions -------------------------------------------------------- */

script static
void BagItem_Tick(struct item *_item) {
   struct bagitem *item = (struct bagitem *)_item;

   for_item(item->content) {
      if(it->Tick) it->Tick(it);
   }
}

script static
void BagItem_Place(struct item *_item, struct container *cont) {
   struct bagitem *item = (struct bagitem *)_item;

   P_Item_Place(&item->item, cont);

   for_item(item->content) {
      if(it->Place) {
         it->Place(it, &item->content);
      }
   }
}

script static
void BagItem_Destroy(struct item *_item) {
   struct bagitem *item = (struct bagitem *)_item;

   for_item(item->content) {
      if(it->Destroy) {
         it->Destroy(it);
      }
   }

   P_Item_Destroy(&item->item);
}

static
bool ItemCanPlace(struct container *cont, struct item *item, i32 x, i32 y) {
   if(x < 0 || y < 0 ||
      x + item->w > cont->w || y + item->h > cont->h) {
      return false;
   }

   for_item(*cont) if(it != item) {
      /* there is certainly a better way of doing this that still
       * accounts for overlap but,
       * damn do i not care
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

   ACS_LocalAmbientSound(ok ? ss_player_cbi_invmov :
                         ss_player_cbi_auto_invalid, 127);
}

static
void Container(struct gui_state *g, struct container *cont, i32 sx, i32 sy) {
   Str(sp_BackStore, s":UI:InvBackStore");
   Str(sp_BackArmsU, s":UI:InvBackUpperArms");
   Str(sp_BackArmsL, s":UI:InvBackLowerArms");
   Str(sp_BackBody,  s":UI:InvBackBody");

   sx += g->ox;
   sy += g->oy;

   str bg;
   switch(cont->type) {
      case _cont_store:  bg = sp_BackStore;  break;
      case _cont_arms_u: bg = sp_BackArmsU; break;
      case _cont_arms_l: bg = sp_BackArmsL; break;
      case _cont_body:   bg = sp_BackBody;   break;
   }

   i32 h = cont->h * 8;
   i32 w = cont->w * 8;

   PrintText_str(Language(LANG "CONTAINER_%s", cont->name), sf_smallfnt, g->defcr, sx,1, sy,2);

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
      ACS_LocalAmbientSound(ss_player_cbi_invmov, 127);
   }

   for_item(*cont) {
      i32 x = sx + it->x * 8;
      i32 y = sy + it->y * 8;
      i32 ex = it->w * 8;
      i32 ey = it->h * 8;

      PrintSprite(it->spr, x,1, y,1);

      if(pl.movitem) {
         continue;
      }

      if(pl.selitem != it && g->clicklft && aabb_point(x, y, ex, ey, g->cx, g->cy)) {
         pl.selitem = it;
         ACS_LocalAmbientSound(ss_player_cbi_invcur, 127);
      }

      if(pl.selitem == it) {
         k32 a = (ACS_Sin(ACS_Timer() / 105.0) * 0.5 + 1.2) / 4;

         ex = ex + x;
         ey = ey + y;

         for(i32 xx = x; y < ey; y += 8) for(x = xx; x < ex; x += 8)
            PrintSpriteA(sp_UI_InvSel, x,1, y,1, a);
      }
   }
}

/* Extern Functions -------------------------------------------------------- */

void P_Inv_PInit() {
   static struct container const baseinv[] = {
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

   fastmemmove(pl.inv, baseinv, sizeof baseinv);

   for(i32 i = 0; i < _inv_num; i++) {
      ListCtor(&pl.inv[i].items);
   }

   pl.invinit = true;
}

void P_Inv_PQuit() {
   for(i32 i = 0; i < _inv_num; i++) {
      for_item(pl.inv[i]) if(it->Destroy) it->Destroy(it);
   }

   pl.useitem = pl.selitem = nil;
   pl.movitem = false;

   pl.invinit = false;
}

void P_Item_Init(struct item *item, struct itemdata const *data) {
   ListCtor(&item->link, item);

   if(data) item->data = *data;
   #ifndef NDEBUG
   else     Log("%s ERROR: invalid item");
   #endif

   if(!item->Destroy) { item->Destroy = P_Item_Destroy; }
   if(!item->Place)   { item->Place   = P_Item_Place;   }
}

struct item *P_Item_New(struct itemdata const *data) {
   struct item *item = Salloc(struct item, _tag_item);

   P_Item_Init(item, data);

   return item;
}

script
void P_Item_Destroy(struct item *item) {
   Dbg_Log(log_dev, "P_Item_Destroy: destroying item %p", item);

   ServCallI(sm_DeleteItem, item);

   if(!P_None()) {
      if(pl.useitem == item) { pl.useitem = nil; }
      if(pl.selitem == item) { pl.selitem = nil; }
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
   item->link.link(&cont->items);
   item->container = cont;
}

void P_Item_Unlink(struct item *item) {
   if(item->container) {
      item->link.unlink();
      item->container = nil;
   }
}

struct bagitem *P_BagItem_New(i32 w, i32 h, i32 type,
                              struct itemdata const *data) {
   struct bagitem *item = Salloc(struct bagitem, _tag_item);

   P_Item_Init(&item->item, data);

   ListCtor(&item->link, item);

   item->content.w    = w;
   item->content.h    = h;
   item->content.type = type;

   item->Tick    = BagItem_Tick;
   item->Destroy = BagItem_Destroy;
   item->Place   = BagItem_Place;

   return item;
}

bool P_Inv_Place(struct container *cont, struct item *item, i32 x, i32 y) {
   if(!ItemCanPlace(cont, item, x, y)) {
      return false;
   }

   if(item->Place) {
      item->Place(item, cont);
   }

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
   for_item(*cont) {
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
         ACS_LocalAmbientSound(ss_player_cbi_auto_invalid, 127);
      }

      pl.useitem = nil;
   }

   for(i32 i = 0; i < _inv_num; i++) {
      for_item(pl.inv[i]) {
         if(it->Tick) {
            it->Tick(it);
         }
      }
   }
}

void P_CBI_TabItems(struct gui_state *g) {
   static i32 const x[] = {
      [_inv_backpack]    = 142+8*-14,
      [_inv_arm_upper_l] = 142+8*  1,
      [_inv_arm_upper_r] = 142+8*  8,
      [_inv_arm_lower_l] = 142+8*  0,
      [_inv_arm_lower_r] = 142+8*  9,
      [_inv_belt]        = 142+8*  3,
      [_inv_leg_l]       = 142+8*  1,
      [_inv_leg_r]       = 142+8*  8,
      [_inv_torso]       = 142+8*  3,
      [_inv_legs]        = 142+8*  4,
   };

   static i32 const y[] = {
      [_inv_backpack]    = 67+8*-1,
      [_inv_arm_upper_l] = 67+8*-2,
      [_inv_arm_upper_r] = 67+8*-2,
      [_inv_arm_lower_l] = 67+8* 3,
      [_inv_arm_lower_r] = 67+8* 3,
      [_inv_belt]        = 67+8* 5,
      [_inv_leg_l]       = 67+8* 7,
      [_inv_leg_r]       = 67+8* 7,
      [_inv_torso]       = 67+8* 0,
      [_inv_legs]        = 67+8*10,
   };

   Str(sl_inv_hints, sLANG "INV_HINTS");

   PrintSpriteA(sp_UI_Body, g->ox+138,1, g->oy+27,1, 0.6);
   PrintSpriteA(sp_UI_Bag,  g->ox+34 ,1, g->oy+31,1, 0.6);

   PrintText_str(L(sl_inv_hints), sf_smallfnt, g->defcr, g->ox+2,1,
                 g->oy+212,2);

   for(i32 i = 0; i < _inv_num; i++) {
      Container(g, &pl.inv[i], x[i], y[i]);
   }

   struct item *sel = pl.selitem;

   if(sel) {
      i32 x_ = x[0];
      i32 y_ = y[0] + 60;

      PrintText_str(Language(LANG "ITEM_TAG_%S", sel->name), sf_smallfnt,
                    g->defcr, g->ox+x_,1, g->oy+y_,1);
      y_ += 8;

      if(g->clickrgt && !g->old.clickrgt) {
         pl.movitem = !pl.movitem;
      }

      PrintText_str(Language(LANG "ITEM_SHORT_%S", sel->name), sf_smallfnt,
                    g->defcr, g->ox+x_,1, g->oy+y_,1);
      y_ += 16;

      if(G_Button(g, LC(LANG "MOVE"), x_, y_, .color = "n", Pre(btnclear))) {
         pl.movitem = !pl.movitem;
      }
      y_ += 8;

      if(sel->equip != _cont_store) {
         if(G_Button(g, LC(LANG "EQUIP"), x_, y_, .color = "n",
                     Pre(btnclear))) {
            EquipItem(sel);
         }
         y_ += 8;
      }

      if(sel->Use) {
         if(G_Button(g, LC(LANG "USE"), x_, y_, .color = "g", Pre(btnclear))) {
            pl.useitem = sel;
         }
         y_ += 8;
      }

      if(sel->scr) {
         PrintTextFmt("(%s\Cnscr\C-)", scoresep(sel->scr));
         PrintText(sf_smallfnt, g->defcr, g->ox+x_+18,1, g->oy+y_,1);
      }

      if(G_Button(g, sel->scr ? LC(LANG "SELL") : LC(LANG "DISCARD"), x_, y_,
                  .color = "g", .fill = {&CBIState(g)->itemfill, 26},
                  Pre(btnclear))) {
         if(sel->scr) {
            P_Scr_Give(sel->scr, true);
         }
         if(sel->Destroy) {
            sel->Destroy(sel);
         }
         ACS_LocalAmbientSound(ss_player_cbi_invrem, 127);
      }
   }
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "ItemCreate")
struct item *Sc_ItemCreate(u32 w, u32 h, u32 equip, u32 scr) {
   str name = GetMembS(0, sm_InvName);
   str spr  = StrParam(":ItemSpr:%S", name);

   Dbg_Log(log_dev, "%s: creating %S", __func__, name);

   struct itemdata const data = {
      .w     = w,
      .h     = h,
      .equip = equip,
      .scr   = scr,
      .spr   = spr,
      .name  = name,
   };

   return P_Item_New(&data);
}

script_str ext("ACS") addr(OBJ "ItemAttach")
bool Sc_ItemAttach(struct item *item) {
   Dbg_Log(log_dev, "%s: attaching item %p", __func__, item);

   if(!P_None()) {
      return P_Inv_Add(item);
   }

   return false;
}

script_str ext("ACS") addr(OBJ "ItemDetach")
void Sc_ItemDetach(struct item *item) {
   Dbg_Log(log_dev, "%s: detaching item %p", __func__, item);

   if(item->Destroy) {
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
