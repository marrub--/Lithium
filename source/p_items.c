/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
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

script
static void BagItem_Tick(struct item *_item)
{
   struct bagitem *item = (struct bagitem *)_item;

   for_item(item->content)
      if(it->Tick) it->Tick(it);
}

script
static void BagItem_Place(struct item *_item, struct container *cont)
{
   struct bagitem *item = (struct bagitem *)_item;

   P_Item_Place(&item->item, cont);

   item->content.user = item->user;

   for_item(item->content)
      if(it->Place) it->Place(it, &item->content);
}

script
static void BagItem_Destroy(struct item *_item)
{
   struct bagitem *item = (struct bagitem *)_item;

   for_item(item->content)
      if(it->Destroy) it->Destroy(it);

   P_Item_Destroy(&item->item);
}

static bool ItemCanPlace(struct container *cont, struct item *item, i32 x, i32 y)
{
   if(x < 0 || y < 0)
      return false;

   i32 x2 = x + item->w;
   i32 y2 = y + item->h;

   if(x2 > cont->w || y2 > cont->h)
      return false;

   i32 z = x + item->w;
   i32 w = y + item->h;

   for_item(*cont) if(it != item) {
      i32 const u = it->x;
      i32 const v = it->y;
      i32 const s = u + it->w;
      i32 const t = v + it->h;

      if(aabb(u, v, s, t, x, y) || aabb(u, v, s, t, z - 1, w - 1) ||
         aabb(x, y, z, w, u, v) || aabb(x, y, z, w, s - 1, t - 1))
         return false;
   }

   return true;
}

static bool ItemCanPlaceAny(struct container *cont, struct item *item)
{
   i32 xn = cont->w / item->w;
   i32 yn = cont->h / item->h;

   for(i32 y = 0; y < yn; y++) for(i32 x = 0; x < xn; x++)
      if(ItemCanPlace(cont, item, x * item->w, y * item->h))
         return true;

   return false;
}

static void Container(struct gui_state *g, struct container *cont, i32 sx, i32 sy)
{
   Str(back_store,  s":UI:InvBackStore");
   Str(back_arms_u, s":UI:InvBackUpperArms");
   Str(back_arms_l, s":UI:InvBackLowerArms");
   Str(back_body,   s":UI:InvBackBody");

   struct player *p = cont->user;

   str bg;
   switch(cont->type) {
      case _cont_store:  bg = back_store;  break;
      case _cont_arms_u: bg = back_arms_u; break;
      case _cont_arms_l: bg = back_arms_l; break;
      case _cont_body:   bg = back_body;   break;
   }

   i32 h = cont->h * 8;
   i32 w = cont->w * 8;

   PrintText_str(Language(LANG "CONTAINER_%s", cont->name), s_smallfnt, CR_WHITE, sx,1, sy,2);

   for(i32 y = 0; y < h; y += 8) for(i32 x = 0; x < w; x += 8)
      PrintSpriteA(bg, sx+x,1, sy+y,1, 0.8);

   if(p && p->movitem && g->clicklft &&
      aabb(sx, sy, sx+w, sy+h, g->cx, g->cy) &&
      (P_Inv_Place(cont, p->selitem, (g->cx - sx) / 8, (g->cy - sy) / 8) ||
       P_Inv_PlaceFirst(cont, p->selitem))) {
      p->movitem = false;
      ACS_LocalAmbientSound(ss_player_cbi_invmov, 127);
   }

   for_item(*cont) {
      i32 x = sx + it->x * 8;
      i32 y = sy + it->y * 8;
      i32 ex = x + it->w * 8;
      i32 ey = y + it->h * 8;

      PrintSprite(it->spr, x,1, y,1);

      if(!p || p->movitem) continue;

      if(p->selitem != it && g->clicklft && aabb(x, y, ex, ey, g->cx, g->cy)) {
         p->selitem = it;
         ACS_LocalAmbientSound(ss_player_cbi_invcur, 127);
      }

      if(p->selitem == it) {
         k32 a = (ACS_Sin(ACS_Timer() / 105.0) * 0.5 + 1.2) / 4;

         for(i32 xx = x; y < ey; y += 8) for(x = xx; x < ex; x += 8)
            PrintSpriteA(sp_UI_InvSel, x,1, y,1, a);
      }
   }
}

/* Extern Functions -------------------------------------------------------- */

void P_Inv_PInit(struct player *p)
{
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

   memmove(p->inv, baseinv, sizeof baseinv);

   for(i32 i = 0; i < _inv_num; i++) {
      ListCtor(&p->inv[i].items);
      p->inv[i].user = p;
   }

   p->invinit = true;
}

void P_Inv_PQuit(struct player *p)
{
   for(i32 i = 0; i < _inv_num; i++)
   {
      for_item(p->inv[i]) if(it->Destroy) it->Destroy(it);
      p->inv[i].user = nil;
   }

   p->useitem = p->selitem = nil;
   p->movitem = false;

   p->invinit = false;
}

void P_Item_Init(struct item *item, struct itemdata const *data)
{
   ListCtor(&item->link, item);

   if(data) item->data = *data;
   else     Log("invalid item, developer is an idiot");

   if(!item->Destroy) item->Destroy = P_Item_Destroy;
   if(!item->Place  ) item->Place   = P_Item_Place;
}

struct item *P_Item_New(struct itemdata const *data)
{
   struct item *item = Salloc(struct item);

   P_Item_Init(item, data);

   return item;
}

script
void P_Item_Destroy(struct item *item)
{
   Dbg_Log(log_dev, "P_Item_Destroy: destroying item %p", item);

   ServCallI(sm_DeleteItem, item);

   with_player(item->user)
   {
      if(p->useitem == item) p->useitem = nil;
      if(p->selitem == item) p->selitem = nil;
      p->movitem = false;
   }

   P_Item_Unlink(item);
   Dalloc(item);
}

script
bool P_Item_Use(struct item *item)
{
   return ServCallI(sm_UseItem, item);
}

script
void P_Item_Place(struct item *item, struct container *cont)
{
   P_Item_Unlink(item);
   item->link.link(&cont->items);
   item->container = cont;
   item->user = cont->user;
}

void P_Item_Unlink(struct item *item)
{
   if(item->container) {
      item->link.unlink();
      item->container = nil;
   }
}

struct bagitem *P_BagItem_New(i32 w, i32 h, i32 type, struct itemdata const *data)
{
   struct bagitem *item = Salloc(struct bagitem);

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

bool P_Inv_Place(struct container *cont, struct item *item, i32 x, i32 y)
{
   if(!ItemCanPlace(cont, item, x, y)) return false;

   if(item->Place) item->Place(item, cont);

   item->x = x;
   item->y = y;

   return true;
}

script
bool P_Inv_PlaceFirst(struct container *cont, struct item *item)
{
   for(i32 y = 0; y < cont->h; y++) for(i32 x = 0; x < cont->w; x++)
      if(P_Inv_Place(cont, item, x, y))
         return true;

   return false;
}

bool P_Inv_Add(struct player *p, struct item *item)
{
   for(i32 i = 0; i < _inv_num; i++)
      if(P_Inv_PlaceFirst(&p->inv[i], item))
         return true;

   return false;
}

script
void P_Inv_PTick(struct player *p)
{
   if(p->useitem) {
      struct item *item = p->useitem;

      Dbg_Log(log_dev, "using %S (%p)", item->name, item);
      if(item->Use && !item->Use(item))
         ACS_LocalAmbientSound(ss_player_cbi_auto_invalid, 127);

      p->useitem = nil;
   }

   for(i32 i = 0; i < _inv_num; i++)
      for_item(p->inv[i])
         if(it->Tick) it->Tick(it);
}

void P_CBI_TabItems(struct gui_state *g, struct player *p)
{
   static i32 const x[] = {
      [_inv_backpack]    = 155+8*-14,
      [_inv_arm_upper_l] = 155+8*  1,
      [_inv_arm_upper_r] = 155+8*  8,
      [_inv_arm_lower_l] = 155+8*  0,
      [_inv_arm_lower_r] = 155+8*  9,
      [_inv_belt]        = 155+8*  3,
      [_inv_leg_l]       = 155+8*  1,
      [_inv_leg_r]       = 155+8*  8,
      [_inv_torso]       = 155+8*  3,
      [_inv_legs]        = 155+8*  4,
   };

   static i32 const y[] = {
      [_inv_backpack]    = 80+8*-1,
      [_inv_arm_upper_l] = 80+8*-2,
      [_inv_arm_upper_r] = 80+8*-2,
      [_inv_arm_lower_l] = 80+8* 3,
      [_inv_arm_lower_r] = 80+8* 3,
      [_inv_belt]        = 80+8* 5,
      [_inv_leg_l]       = 80+8* 7,
      [_inv_leg_r]       = 80+8* 7,
      [_inv_torso]       = 80+8* 0,
      [_inv_legs]        = 80+8*10,
   };

   PrintSpriteA(sp_UI_Body, 151,1, 40,1, 0.6);
   PrintSpriteA(sp_UI_Bag,  47 ,1, 44,1, 0.6);

   PrintTextChS(LC(LANG "INV_HINTS"));
   PrintText(s_smallfnt, CR_WHITE, 15,1, 225,2);

   for(i32 i = 0; i < _inv_num; i++)
      Container(g, &p->inv[i], x[i], y[i]);

   struct item *sel = p->selitem;

   if(sel) {
      i32 x_ = x[0];
      i32 y_ = y[0] + 60;

      PrintText_str(Language(LANG "ITEM_TAG_%S", sel->name), s_smallfnt, CR_WHITE, x_,1, y_,1);
      y_ += 8;

      if(g->clickrgt && !g->old.clickrgt)
         p->movitem = !p->movitem;

      PrintText_str(Language(LANG "ITEM_SHORT_%S", sel->name), s_smallfnt, CR_WHITE, x_,1, y_,1);
      y_ += 16;

      if(G_Button(g, LC(LANG "MOVE"), x_, y_, .color = "n", Pre(btnclear)))
         p->movitem = !p->movitem;
      y_ += 8;

      if(sel->Use) {
         if(G_Button(g, LC(LANG "USE"), x_, y_, .color = "g", Pre(btnclear)))
            p->useitem = sel;
         y_ += 8;
      }

      if(sel->scr) {
         PrintTextFmt("(%s\Cnscr\C-)", scoresep(sel->scr));
         PrintText(s_smallfnt, CR_WHITE, x_+18,1, y_,1);
      }

      if(G_Button(g, sel->scr ? LC(LANG "SELL") : LC(LANG "DISCARD"), x_, y_, .color = "g", Pre(btnclear))) {
         if(sel->scr) P_Scr_Give(p, sel->scr, true);
         if(sel->Destroy) sel->Destroy(sel);
         ACS_LocalAmbientSound(ss_player_cbi_invrem, 127);
      }
   }
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr("Lith_ItemCreate")
struct item *Sc_ItemCreate(i32 w, i32 h)
{
   str type = GetMembS(0, sm_InvType);
   str tag  = GetMembS(0, sm_InvName);
   u32 scr  = GetMembI(0, sm_InvSell);
   str spr  = StrParam(":ItemSpr:%S", tag);
   str name = GetMembS(0, sm_InvName);

   Dbg_Log(log_dev, "%s: creating %S (%S) %S", __func__, type, tag, spr);

   #define Type(t, ...) \
      Str(t##s, s"" #t); \
      if(type == t##s) { \
         struct itemdata const data = \
            {name, spr, tag, w, h, scr, __VA_ARGS__}; \
         return P_Item_New(&data); \
      }

   Type(Armor);

   return nil;
}

script_str ext("ACS") addr("Lith_ItemAttach")
bool Sc_ItemAttach(struct item *item)
{
   Dbg_Log(log_dev, "%s: attaching item %p", __func__, item);

   with_player(LocalPlayer) return P_Inv_Add(p, item);

   return false;
}

script_str ext("ACS") addr("Lith_ItemDetach")
void Sc_ItemDetach(struct item *item)
{
   Dbg_Log(log_dev, "%s: detaching item %p", __func__, item);

   if(item->Destroy) item->Destroy(item);
}

script_str ext("ACS") addr("Lith_ItemCanPlace")
bool Sc_ItemCanPlace(struct item *item)
{
   with_player(LocalPlayer)
      for(i32 i = 0; i < _inv_num; i++)
         if(ItemCanPlaceAny(&p->inv[i], item))
            return true;

   return false;
}

script_str ext("ACS") addr("Lith_ItemOnBody")
bool Sc_ItemOnBody(struct item *item)
{
   return item->container && item->container->type == _cont_body;
}

/* EOF */
