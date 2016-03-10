#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_cbi_gui.h"

// ---------------------------------------------------------------------------
// Node Functions.
//

int CBI_NodeListDraw(dlist_t *list, int id)
{
   int ret = 0;
   
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      cbi_node_t *node = rover->data.vp;
      
      if(node->visible && node->Draw)
      {
         int i = node->Draw(node, id);
         id -= i;
         ret += i;
      }
      
      if(id < hid_cbi_underflow)
         Log("Hud ID underflow in CBI_NodeListDraw!");
   }
   
   return ret;
}

void CBI_NodeListUpdate(dlist_t *list, player_t *p, struct cursor_s cur)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      cbi_node_t *node = rover->data.vp;
      
      if(node->visible && node->Update)
         node->Update(node, p, cur);
   }
}

bool CBI_NodeListClick(dlist_t *list, player_t *p, struct cursor_s cur, bool left)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      cbi_node_t *node = rover->data.vp;
      
      if(node->visible && node->Click)
         if(node->Click(node, p, cur, left))
            return true;
   }
   
   return false;
}

bool CBI_NodeListHold(dlist_t *list, player_t *p, struct cursor_s cur, bool left)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      cbi_node_t *node = rover->data.vp;
      
      if(node->visible && node->Hold)
         if(node->Hold(node, p, cur, left))
            return true;
   }
   
   return false;
}

// ---------------------------------------------------------------------------
// cbi_node_t
//

int CBI_NodeDraw(cbi_node_t *node, int id)
{
   if(node->children)
      return CBI_NodeListDraw(node->children, id);
   
   return 0;
}

void CBI_NodeUpdate(cbi_node_t *node, player_t *p, struct cursor_s cur)
{
   if(node->children)
      CBI_NodeListUpdate(node->children, p, cur);
}

bool CBI_NodeClick(cbi_node_t *node, player_t *p, struct cursor_s cur, bool left)
{
   if(node->children)
      return CBI_NodeListClick(node->children, p, cur, left);
   
   return false;
}

bool CBI_NodeHold(cbi_node_t *node, player_t *p, struct cursor_s cur, bool left)
{
   Log("CBI_NodeHold %p", node);
   
   if(node->children)
      return CBI_NodeListHold(node->children, p, cur, left);
   
   return false;
}

cbi_node_t *CBI_NodeAlloc(int flags, int id, int x, int y)
{
   cbi_node_t *node = calloc(1, sizeof(cbi_node_t));
   
   node->visible = !(flags & NODEAF_NOTVISIBLE);
   node->x = x;
   node->y = y;
   node->id = id;
   node->Draw = CBI_NodeDraw;
   node->Update = CBI_NodeUpdate;
   node->Click = CBI_NodeClick;
   node->Hold = CBI_NodeHold;
   
   return node;
}

// ---------------------------------------------------------------------------
// cbi_text_t
//

int CBI_TextDraw(cbi_node_t *node, int id)
{
   cbi_text_t *text = (cbi_text_t *)node;
   cbi_node_t *node = &text->node;
   int ret = 0;
   
   // The most important check.
   if(text->rainbows)
      HudMessageRainbowsF(text->font, "%S", text->text);
   else
      HudMessageF(text->font, "%S", text->text);
   
   HudMessagePlain(id, 0.1 + node->x, 0.1 + node->y, TICSECOND);
   ret++;
   id--;
   
   ret += CBI_NodeDraw(node, id);
   return ret;
}

cbi_node_t *CBI_TextAlloc(int flags, int id, int x, int y, __str text, __str font)
{
   cbi_text_t *node = calloc(1, sizeof(cbi_text_t));
   
   node->text = text;
   node->font = font ? font : "CBIFONT";
   node->rainbows = flags & TXTAF_RAINBOWS;
   node->node.visible = !(flags & TXTAF_NOTVISIBLE);
   node->node.x = x;
   node->node.y = y;
   node->node.id = id;
   node->node.Draw = CBI_TextDraw;
   node->node.Update = CBI_NodeUpdate;
   node->node.Click = CBI_NodeClick;
   node->node.Hold = CBI_NodeHold;
   
   return &node->node;
}

// ---------------------------------------------------------------------------
// cbi_sprite_t
//

int CBI_SpriteDraw(cbi_node_t *node, int id)
{
   cbi_sprite_t *sprite = (cbi_sprite_t *)node;
   cbi_node_t *node = &sprite->node;
   int ret = 0;
   
   DrawSprite(sprite->name, HUDMSG_ALPHA, id, 0.1 + node->x, 0.1 + node->y, TICSECOND, sprite->alpha);
   ret++;
   id--;
   
   ret += CBI_NodeDraw(node, id);
   return ret;
}

cbi_node_t *CBI_SpriteAlloc(int flags, int id, int x, int y, __str name, fixed alpha)
{
   cbi_sprite_t *node = calloc(1, sizeof(cbi_sprite_t));
   
   node->name = name;
   node->alpha = (flags & SPRAF_ALPHA) ? alpha : 1.0;
   node->node.visible = !(flags & SPRAF_NOTVISIBLE);
   node->node.x = x;
   node->node.y = y;
   node->node.id = id;
   node->node.Draw = CBI_SpriteDraw;
   node->node.Update = CBI_NodeUpdate;
   node->node.Click = CBI_NodeClick;
   node->node.Hold = CBI_NodeHold;
   
   return &node->node;
}

// ---------------------------------------------------------------------------
// cbi_button_t
//

int CBI_ButtonDraw(cbi_node_t *node, int id)
{
   cbi_button_t *button = (cbi_button_t *)node;
   cbi_node_t *node = &button->node;
   int ret = 0;
   
   if(button->label)
   {
      char color = 'j';
      
      if(button->clicked)
         color = 'g';
      else if(button->hover)
         color = 'k';
      
      HudMessageF("CBIFONT", "\C%c%S", color, button->label);
      HudMessagePlain(id - 1, node->x + (48 / 2), node->y + (16 / 2), TICSECOND);
      ret += 2;
   }
   else
      ret++;
   
   DrawSprite("H_Z3", HUDMSG_ALPHA, id, 0.1 + node->x, 0.1 + node->y, TICSECOND, 0.7);
   
   if(button->clicked)
      button->clicked--;
   
   ret += CBI_NodeDraw(node, id);
   return ret;
}

void CBI_ButtonUpdate(cbi_node_t *node, player_t *p, struct cursor_s cur)
{
   cbi_button_t *button = (cbi_button_t *)node;
   cbi_node_t *node = &button->node;
   
   CBI_NodeUpdate(node, p, cur);
   
   if(cur.x >= node->x + 48 || cur.y >= node->y + 16 ||
      cur.x < node->x || cur.y < node->y)
      button->hover = false;
   else
      button->hover = true;
}

bool CBI_ButtonClick(cbi_node_t *node, player_t *p, struct cursor_s cur, bool left)
{
   cbi_button_t *button = (cbi_button_t *)node;
   cbi_node_t *node = &button->node;
   
   if(CBI_NodeClick(node, p, cur, left))
      return true;
   
   if(cur.x >= node->x + 48 || cur.y >= node->y + 16 ||
      cur.x < node->x || cur.y < node->y)
      return false;
   
   // TODO: this could possibly cause data race conditions, go ask david
   bool ret = false;
   if(button->Event)
      button->Event(button, p, left, &ret);
   
   if(ret)
   {
      button->clicked = 5;
      ACS_LocalAmbientSound("player/cbi/buttonpress", 127);
   }
   
   return ret;
}

cbi_node_t *CBI_ButtonAlloc(int flags, int id, int x, int y, cbi_buttonevent_t event, __str label)
{
   cbi_button_t *node = calloc(1, sizeof(cbi_button_t));
   
   node->label = label;
   node->Event = event;
   node->node.visible = !(flags & BTNAF_NOTVISIBLE);
   node->node.x = x;
   node->node.y = y;
   node->node.id = id;
   node->node.Draw = CBI_ButtonDraw;
   node->node.Update = CBI_ButtonUpdate;
   node->node.Click = CBI_ButtonClick;
   node->node.Hold = CBI_NodeHold;
   
   return &node->node;
}

// ---------------------------------------------------------------------------
// cbi_tab_t
//

int CBI_TabDraw(cbi_node_t *node, int id)
{
   cbi_tab_t *tab = (cbi_tab_t *)node;
   cbi_node_t *node = &tab->node;
   int ret = 0;
   
   for(int i = 0; i < tab->ntabs; i++)
   {
      char color = 'j';
      
      if(tab->curtab == i && tab->clicked)
         color = 'g';
      else if(tab->hover == i)
         color = 'k';
      else if(tab->curtab == i)
         color = 'n';
      
      HudMessageF("CBIFONT", "\C%c%S", color, tab->names[i]);
      HudMessagePlain(id - 1, node->x + (48 / 2) + (48 * i), node->y + (16 / 2), TICSECOND);
      DrawSprite("H_Z3", HUDMSG_ALPHA, id, 0.1 + node->x + (48 * i), 0.1 + node->y, TICSECOND, 0.7);
      
      ret += 2;
      id -= 2;
   }
   
   if(tab->clicked)
      tab->clicked--;
   
   ret += CBI_NodeDraw(node, id);
   return ret;
}

void CBI_TabUpdate(cbi_node_t *node, player_t *p, struct cursor_s cur)
{
   cbi_tab_t *tab = (cbi_tab_t *)node;
   cbi_node_t *node = &tab->node;
   
   tab->hover = -1;
   
   if(!node->children)
      return;
   
   int i = 0;
   for(slist_t *rover = node->children->head; rover; rover = rover->next, i++)
   {
      cbi_node_t *rnode = rover->data.vp;
      rnode->visible = (i == tab->curtab);
      
      int x = node->x + (48 * i);
      if(cur.x < x + 48 && cur.y < node->y + 16 && cur.x >= x && cur.y >= node->y)
         tab->hover = i;
   }
   
   CBI_NodeUpdate(node, p, cur);
}

bool CBI_TabClick(cbi_node_t *node, player_t *p, struct cursor_s cur, bool left)
{
   cbi_tab_t *tab = (cbi_tab_t *)node;
   cbi_node_t *node = &tab->node;
   
   if(CBI_NodeClick(node, p, cur, left))
      return true;
   
   if(!left)
      return false;
   
   for(int i = 0; i < tab->ntabs; i++)
   {
      int x = node->x + (48 * i);
      
      if(cur.x < x + 48 && cur.y < node->y + 16 && cur.x >= x && cur.y >= node->y)
      {
         ACS_LocalAmbientSound("player/cbi/buttonpress", 127);
         tab->curtab = i;
         tab->clicked = 5;
         return true;
      }
   }
   
   return false;
}

cbi_node_t *CBI_TabAlloc(int flags, int id, int x, int y, __str *names)
{
   cbi_tab_t *node = calloc(1, sizeof(cbi_tab_t));
   
   for(__str *p = names; *p; p++)
      node->ntabs++;
   
   node->names = names;
   node->node.visible = !(flags & TABAF_NOTVISIBLE);
   node->node.x = x;
   node->node.y = y;
   node->node.id = id;
   node->node.Draw = CBI_TabDraw;
   node->node.Update = CBI_TabUpdate;
   node->node.Click = CBI_TabClick;
   node->node.Hold = CBI_NodeHold;
   
   return &node->node;
}

//
// ---------------------------------------------------------------------------

