// Copyright © 2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

// Static Functions ----------------------------------------------------------|

//
// BagItem_Tick
//
[[__call("ScriptS")]]
static void BagItem_Tick(item_t *_item)
{
   bagitem_t *item = (bagitem_t *)_item;

   foritem(item->content)
      if(it->Tick) it->Tick(it);
}

//
// BagItem_Destroy
//
[[__call("ScriptS")]]
static void BagItem_Destroy(item_t *_item)
{
   bagitem_t *item = (bagitem_t *)_item;

   foritem(item->content)
      it->Destroy(it);

   Lith_Item_Destroy(&item->item);
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_Item_New
//
item_t *Lith_Item_New(itemdata_t const *data)
{
   item_t *item = salloc(item_t);

   item->link.construct(item);

   if(data)
      item->data = *data;

   if(!item->Destroy)
      item->Destroy = Lith_Item_Destroy;

   return item;
}

//
// Lith_Item_Destroy
//
[[__call("ScriptS")]]
void Lith_Item_Destroy(item_t *item)
{
   item->link.unlink();
   free(item);
}

//
// Lith_BagItem_New
//
bagitem_t *Lith_BagItem_New(int w, int h, __str bg)
{
   bagitem_t *item = salloc(bagitem_t);

   item->link.construct(item);

   item->content.w  = w;
   item->content.h  = h;
   item->content.bg = bg;

   item->Tick    = BagItem_Tick;
   item->Destroy = BagItem_Destroy;

   return item;
}

//
// Lith_ItemPlace
//
bool Lith_ItemPlace(container_t *cont, item_t *item, int x, int y)
{
   if(x < 0 || y < 0)
      return false;

   int x2 = x + item->w;
   int y2 = y + item->h;

   if(x2 > cont->w || y2 > cont->h)
      return false;

   foritem(*cont)
      if(x > it->x && x < it->x + it->w && y > it->y && y < it->y + it->h)
         return false;

   if(item->container)
      item->link.unlink();

   item->link.link(&cont->items);
   item->container = cont;

   item->x = x;
   item->y = y;

   return true;
}

//
// Lith_Container
//
void Lith_Container(gui_state_t *g, container_t *cont, int sx, int sy)
{
   __str bg = cont->bg;
   int h = cont->h * 8;
   int w = cont->w * 8;

   for(int y = 0; y < h; y += 8)
      for(int x = 0; x < w; x += 8)
         DrawSpritePlain(bg, g->hid--, sx+x+.1, sy+y+.1, TS);

   foritem(*cont)
      if(it->Tick) it->Tick(it);

   fixed a = ACS_Sin(ACS_Timer() / 105.0) * 0.5 + 0.6;

   foritem(*cont) {
      DrawSpritePlain(it->spr, g->hid--, sx+it->x*8+.1, sy+it->y*8+.1, TS);

      for(int y = it->y*8, ey = y+it->h*8; y < ey; y += 8)
         for(int x = it->x*8, ex = x+it->w*8; x < ex; x += 8)
      {
         DrawSpriteX("lgfx/UI/InvSel.png", HUDMSG_ALPHA|HUDMSG_ADDBLEND,
            g->hid--, sx+x+.1, sy+y+.1, TS, a);
      }
   }
}

//
// Lith_CBITab_Items
//
void Lith_CBITab_Items(gui_state_t *g, player_t *p)
{
   static int const bx = 150, by = 80;

   Lith_Container(g, &p->inv[0], bx+8*1, by+8*1);
   Lith_Container(g, &p->inv[1], bx+8*8, by+8*1);
   Lith_Container(g, &p->inv[2], bx+8*0, by+8*5);
   Lith_Container(g, &p->inv[3], bx+8*9, by+8*5);
   Lith_Container(g, &p->inv[4], bx+8*3, by+8*0);
   Lith_Container(g, &p->inv[5], bx+8*2, by+8*9);
   Lith_Container(g, &p->inv[6], bx+8*6, by+8*9);
}

// EOF
