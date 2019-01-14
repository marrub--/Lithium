// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_items.c: Inventory handling and UI.

#if LITHIUM
#include "common.h"
#include "p_player.h"
#include "w_world.h"

// Static Functions ----------------------------------------------------------|

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
      it->Place(it, &item->content);
}

script
static void BagItem_Destroy(struct item *_item)
{
   struct bagitem *item = (struct bagitem *)_item;

   for_item(item->content)
      it->Destroy(it);

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

   for_item(*cont) if(it != item)
   {
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
   struct player *p = cont->user;

   str bg = cont->bg ? cont->bg : sp_UI_InvBack;
   i32 h = cont->h * 8;
   i32 w = cont->w * 8;

   for(i32 y = 0; y < h; y += 8) for(i32 x = 0; x < w; x += 8)
      PrintSpriteA(bg, sx+x,1, sy+y,1, 0.8);

   if(p && p->movitem && g->clicklft && aabb(sx, sy, sx+w, sy+h, g->cx, g->cy))
      if(P_Inv_Place(cont, p->selitem, (g->cx - sx) / 8, (g->cy - sy) / 8))
   {
      p->movitem = false;
      ACS_LocalAmbientSound(ss_player_cbi_invmov, 127);
   }

   for_item(*cont)
   {
      i32 x = sx + it->x * 8;
      i32 y = sy + it->y * 8;
      i32 ex = x + it->w * 8;
      i32 ey = y + it->h * 8;

      PrintSprite(it->spr, x,1, y,1);

      if(!p || p->movitem) continue;

      if(p->selitem != it && g->clicklft && aabb(x, y, ex, ey, g->cx, g->cy))
      {
         p->selitem = it;
         ACS_LocalAmbientSound(ss_player_cbi_invcur, 127);
      }

      if(p->selitem == it)
      {
         k32 a = (ACS_Sin(ACS_Timer() / 105.0) * 0.5 + 1.2) / 4;

         for(i32 xx = x; y < ey; y += 8) for(x = xx; x < ex; x += 8)
            PrintSpriteA(sp_UI_InvSel, x,1, y,1, a);
      }
   }
}

// Extern Functions ----------------------------------------------------------|

void P_Inv_PInit(struct player *p)
{
   static struct container const baseinv[] = {
      {11, 7},
      {1, 3}, {1, 3}, {1, 3}, {1, 3},
      {4, 1},
      {2, 4}, {2, 4},
   };

   memmove(p->inv, baseinv, sizeof baseinv);

   for(i32 i = 0; i < countof(p->inv); i++) {
      ListCtor(&p->inv[i].items);
      p->inv[i].user = p;
   }

   ListCtor(&p->misc.items);
   p->misc.user = p;

   p->invinit = true;
}

void P_Inv_PQuit(struct player *p)
{
   for(i32 i = 0; i < countof(p->inv); i++)
   {
      for_item(p->inv[i]) it->Destroy(it);
      p->inv[i].user = nil;
   }

   for_item(p->misc) it->Destroy(it);
   p->misc.user = nil;

   p->useitem = p->selitem = nil;
   p->movitem = false;
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
   if(item->container)
   {
      item->link.unlink();
      item->container = nil;
   }
}

struct bagitem *P_BagItem_New(i32 w, i32 h, str bg, struct itemdata const *data)
{
   struct bagitem *item = Salloc(struct bagitem);

   P_Item_Init(&item->item, data);

   ListCtor(&item->link, item);

   item->content.w  = w;
   item->content.h  = h;
   item->content.bg = bg;

   item->Tick    = BagItem_Tick;
   item->Destroy = BagItem_Destroy;
   item->Place   = BagItem_Place;

   return item;
}

bool P_Inv_Place(struct container *cont, struct item *item, i32 x, i32 y)
{
   if(!ItemCanPlace(cont, item, x, y)) return false;

   item->Place(item, cont);

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
   for(i32 i = 0; i < countof(p->inv); i++)
      if(P_Inv_PlaceFirst(&p->inv[i], item))
         return true;

   return false;
}

script
void P_Inv_PTick(struct player *p)
{
   if(p->useitem)
   {
      struct item *item = p->useitem;

      Dbg_Log(log_dev, "using %S (%p)", item->name, item);
      if(item->Use && !item->Use(item))
         ACS_LocalAmbientSound(ss_player_cbi_auto_invalid, 127);

      p->useitem = nil;
   }

   for(i32 i = 0; i < countof(p->inv); i++)
      for_item(p->inv[i])
         if(it->Tick) it->Tick(it);

   for_item(p->misc)
      if(it->Tick) it->Tick(it);
}

void P_CBI_TabItems(struct gui_state *g, struct player *p)
{
   static i32 const x[] = {
      155+8*-14, // Backpack
      155+8*  1, // L Upper Arm
      155+8*  8, // R Upper Arm
      155+8*  0, // L Lower Arm
      155+8*  9, // R Lower Arm
      155+8*  3, // Belt
      155+8*  1, // L Leg
      155+8*  7, // R Leg
   };

   static i32 const y[] = {
      80+8*-1, // Backpack
      80+8*-2, // L Upper Arm
      80+8*-2, // R Upper Arm
      80+8* 2, // L Lower Arm
      80+8* 2, // R Lower Arm
      80+8* 5, // Belt
      80+8* 9, // L Leg
      80+8* 9, // R Leg
   };

   PrintSpriteA(sp_UI_Body, 151,1, 40,1, 0.6);
   PrintSpriteA(sp_UI_Bag,  47 ,1, 44,1, 0.6);

   for(i32 i = 0; i < countof(p->inv); i++)
      Container(g, &p->inv[i], x[i], y[i]);

   struct item *sel = p->selitem;

   if(sel)
   {
      i32 x_ = x[0];
      i32 y_ = y[0] + 60;

      PrintText_str(sel->name, s_cbifont, CR_WHITE, x_,1, y_,1);
      y_ += 8;

      if(g->clickrgt && !g->old.clickrgt)
         p->movitem = !p->movitem;

      if(G_Button(g, "Move", x_, y_, Pre(btnclear)))
         p->movitem = !p->movitem;
      y_ += 8;

      if(sel->Use)
      {
         if(G_Button(g, "Use", x_, y_, Pre(btnclear)))
            p->useitem = sel;
         y_ += 8;
      }

      if(sel->scr)
      {
         PrintTextFmt("(%s\Cnscr\C-)", scoresep(sel->scr));
         PrintText(s_cbifont, CR_WHITE, x_+18,1, y_,1);
      }

      if(G_Button(g, sel->scr ? "Sell" : "Discard", x_, y_, Pre(btnclear)))
      {
         if(sel->scr) P_Scr_Give(p, sel->scr, true);
         sel->Destroy(sel);
         ACS_LocalAmbientSound(ss_player_cbi_invrem, 127);
      }
   }

   PrintTextFmt("Equipped (%S)", ServCallS(sm_GetArmorDT));
   PrintText(s_cbifont, CR_WHITE, 40,1, 38,1);

   for(i32 i = 0; i < aslot_max; i++)
      ifw(str name = ServCallS(sm_GetArmorSlot, i), name != s_NIL)
         PrintText_str(name, s_cbifont, CR_WHITE, 40,1, 45+7*i,1);
}

// Scripts -------------------------------------------------------------------|

script_str ext("ACS") addr("Lith_ItemCreate")
void *Sc_ItemCreate(i32 w, i32 h)
{
   str type = GetMembS(0, sm_InvType);
   str tag  = GetMembS(0, sm_InvName);
   u32 scr  = GetMembI(0, sm_InvSell);
   str spr  = StrParam(":ItemSpr:%S", tag);
   str name = GetPropS(0, APROP_NameTag);

   Dbg_Log(log_dev, "%s: creating %S (%S) %S", __func__, type, tag, spr);

   #define Type(t, ...) \
      if(type == t) \
         return P_Item_New(&(struct itemdata const){name, spr, tag, w, h, scr, __VA_ARGS__})

   Type(si_SlottedItem, .Use = P_Item_Use);
   Type(si_Armor,       .Use = P_Item_Use);

   return nil;
}

script_str ext("ACS") addr("Lith_ItemAttach")
bool Sc_ItemAttach(void *_item)
{
   struct item *item = _item;

   Dbg_Log(log_dev, "%s: attaching item %p", __func__, item);

   with_player(LocalPlayer)
   {
      bip_name_t tag; lstrcpy_str(tag, item->tag);
      P_BIP_Unlock(p, tag);
      return P_Inv_Add(p, item);
   }

   return false;
}

script_str ext("ACS") addr("Lith_ItemDetach")
void Sc_ItemDetach(void *_item)
{
   struct item *item = _item;

   Dbg_Log(log_dev, "%s: detaching item %p", __func__, item);

   item->Destroy(item);
}

script_str ext("ACS") addr("Lith_ItemUnlink")
void Sc_ItemUnlink(void *_item)
{
   struct item *item = _item;

   Dbg_Log(log_dev, "%s: unlinking item %p", __func__, item);

   with_player(LocalPlayer)
   {
      item->Place(item, &p->misc);
      item->x = item->y = 0;

      p->selitem = nil;
      p->movitem = false;
   }
}

script_str ext("ACS") addr("Lith_ItemCanPlace")
bool Sc_ItemCanPlace(void *_item)
{
   struct item *item = _item;

   with_player(LocalPlayer)
      for(i32 i = 0; i < countof(p->inv); i++)
         if(ItemCanPlaceAny(&p->inv[i], item))
            return true;

   return false;
}
#endif

// EOF
