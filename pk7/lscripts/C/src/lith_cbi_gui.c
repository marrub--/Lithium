#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_cbi_gui.h"
#include <math.h>

// ---------------------------------------------------------------------------
// Node Functions.
//

int CBI_NodeListDraw(dlist_t *list, int id)
{
   int ret = 0;
   
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      cbi_node_t *node = rover->data.vp;
      
      if(node->visible)
      {
         int i;
         
         i = node->basefuncs.Draw(node, id);
         id -= i, ret += i;
         
         if(node->userfuncs.Draw)
         {
            i = node->userfuncs.Draw(node, id);
            id -= i, ret += i;
         }
      }
      
      if(id < hid_cbi_underflow)
         Log("Hud ID underflow in CBI_NodeListDraw!");
   }
   
   return ret;
}

void CBI_NodeListUpdate(dlist_t *list, player_t *p, cursor_t cur)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      cbi_node_t *node = rover->data.vp;
      
      if(node->visible)
      {
         node->basefuncs.Update(node, p, cur);
         
         if(node->userfuncs.Update)
            node->userfuncs.Update(node, p, cur);
      }
   }
}

bool CBI_NodeListClick(dlist_t *list, player_t *p, cursor_t cur, bool left)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      cbi_node_t *node = rover->data.vp;
      
      if(node->visible)
         if((node->userfuncs.Click && node->userfuncs.Click(node, p, cur, left)) ||
            node->basefuncs.Click(node, p, cur, left))
            return true;
   }
   
   return false;
}

bool CBI_NodeListHold(dlist_t *list, player_t *p, cursor_t cur, bool left)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      cbi_node_t *node = rover->data.vp;
      
      if(node->visible)
         if((node->userfuncs.Hold && node->userfuncs.Hold(node, p, cur, left)) ||
            node->basefuncs.Hold(node, p, cur, left))
            return true;
   }
   
   return false;
}

cbi_node_t *CBI_NodeListGetByID(dlist_t *list, int id)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      cbi_node_t *node = rover->data.vp;
      cbi_node_t *cnode;
      
      if(node->id == id)
         return node;
      else if(node->children)
         if((cnode = CBI_NodeListGetByID(node->children, id)))
            return cnode;
   }
   
   return null;
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

void CBI_NodeUpdate(cbi_node_t *node, player_t *p, cursor_t cur)
{
   if(node->children)
      CBI_NodeListUpdate(node->children, p, cur);
}

bool CBI_NodeClick(cbi_node_t *node, player_t *p, cursor_t cur, bool left)
{
   if(node->children)
      return CBI_NodeListClick(node->children, p, cur, left);
   
   return false;
}

bool CBI_NodeHold(cbi_node_t *node, player_t *p, cursor_t cur, bool left)
{
   if(node->children)
      return CBI_NodeListHold(node->children, p, cur, left);
   
   return false;
}

void CBI_NodeReset(cbi_node_t *node, bool visible, int id, int x, int y, cbi_nodefuncs_t *userfuncs)
{
   node->visible = visible;
   node->id = id;
   node->x = x;
   node->y = y;
   node->basefuncs.Draw   = CBI_NodeDraw;
   node->basefuncs.Update = CBI_NodeUpdate;
   node->basefuncs.Click  = CBI_NodeClick;
   node->basefuncs.Hold   = CBI_NodeHold;
   
   if(userfuncs)
      memcpy(&node->userfuncs, userfuncs, sizeof(cbi_nodefuncs_t));
   else
      memset(&node->userfuncs, 0, sizeof(cbi_nodefuncs_t));
}

cbi_node_t *CBI_NodeAlloc(int flags, int id, int x, int y, cbi_nodefuncs_t *userfuncs)
{
   cbi_node_t *node = calloc(1, sizeof(cbi_node_t));
   CBI_NodeReset(node, !(flags & NODEAF_NOTVISIBLE), id, x, y, userfuncs);
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
   
   if(text->text)
   {
      if(text->rainbows)
         HudMessageRainbowsF(text->font, "%S", text->text);
      else
         HudMessageF(text->font, "%S", text->text);
      
      HudMessagePlain(id, text->alignx + node->x, text->aligny + node->y, TICSECOND);
      id--, ret++;
   }
   
   ret += CBI_NodeDraw(node, id);
   return ret;
}

cbi_node_t *CBI_TextAlloc(int flags, int id, int x, int y, cbi_nodefuncs_t *userfuncs, __str text, __str font)
{
   cbi_text_t *node = calloc(1, sizeof(cbi_text_t));
   
   node->text = text;
   node->font = font ? font : "CBIFONT";
   node->rainbows = flags & TXTAF_RAINBOWS;
   
   if(flags & TXTAF_ALIGNX_CENTER)
      node->alignx = 0.0;
   else if(flags & TXTAF_ALIGNX_RIGHT)
      node->alignx = 0.2;
   else
      node->alignx = 0.1;
   
   if(flags & TXTAF_ALIGNY_CENTER)
      node->aligny = 0.0;
   else if(flags & TXTAF_ALIGNY_BOTTOM)
      node->aligny = 0.2;
   else
      node->aligny = 0.1;
   
   CBI_NodeReset(&node->node, !(flags & NODEAF_NOTVISIBLE), id, x, y, userfuncs);
   node->node.basefuncs.Draw = CBI_TextDraw;
   
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
   
   if(sprite->name)
   {
      DrawSpriteAlpha(sprite->name, id,
         sprite->alignx + node->x,
         sprite->aligny + node->y,
         TICSECOND, sprite->alpha);
      id--, ret++;
   }
   
   ret += CBI_NodeDraw(node, id);
   return ret;
}

cbi_node_t *CBI_SpriteAlloc(int flags, int id, int x, int y, cbi_nodefuncs_t *userfuncs, __str name, fixed alpha)
{
   cbi_sprite_t *node = calloc(1, sizeof(cbi_sprite_t));
   
   node->name = name;
   node->alpha = (flags & SPRAF_ALPHA) ? alpha : 1.0;
   
   if(flags & SPRAF_ALIGNX_CENTER)
      node->alignx = 0.0;
   else if(flags & SPRAF_ALIGNX_RIGHT)
      node->alignx = 0.2;
   else
      node->alignx = 0.1;
   
   if(flags & SPRAF_ALIGNY_CENTER)
      node->aligny = 0.0;
   else if(flags & SPRAF_ALIGNY_BOTTOM)
      node->aligny = 0.2;
   else
      node->aligny = 0.1;
   
   CBI_NodeReset(&node->node, !(flags & NODEAF_NOTVISIBLE), id, x, y, userfuncs);
   node->node.basefuncs.Draw = CBI_SpriteDraw;
   
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
   
   DrawSpriteAlpha("H_Z3", id, 0.1 + node->x, 0.1 + node->y, TICSECOND, 0.7);
   id--, ret++;
   
   if(button->label)
   {
      char color = 'j';
      
      if(button->clicked < 0)
         color = 'f';
      else if(button->clicked)
         color = 'g';
      
      HudMessageF(button->font, "\C%c%S", color, button->label);
      HudMessagePlain(id, (CBI_BUTTON_W / 2) + node->x, (CBI_BUTTON_H / 2) + node->y, TICSECOND);
      
      id--, ret++;
   }
   
   ret += CBI_NodeDraw(node, id);
   return ret;
}

void CBI_ButtonUpdate(cbi_node_t *node, player_t *p, cursor_t cur)
{
   cbi_button_t *button = (cbi_button_t *)node;
   cbi_node_t *node = &button->node;
   
   if(button->clicked < 0)
      button->clicked = 0;
   else if(button->clicked > 0)
      button->clicked--;
   
   if(bpcldi(node->x, node->y, node->x + CBI_BUTTON_W, node->y + CBI_BUTTON_H, cur.x, cur.y) &&
      button->clicked == 0)
      button->clicked = -1;
   
   CBI_NodeUpdate(node, p, cur);
}

bool CBI_ButtonClick(cbi_node_t *node, player_t *p, cursor_t cur, bool left)
{
   cbi_button_t *button = (cbi_button_t *)node;
   cbi_node_t *node = &button->node;
   
   if(CBI_NodeClick(node, p, cur, left))
      return true;
   
   if((left  && !(button->respond == 1 || button->respond == 3)) ||
      (!left && !(button->respond == 2 || button->respond == 3)))
      return false;
   
   if(bpcldi(node->x, node->y, node->x + CBI_BUTTON_W, node->y + CBI_BUTTON_H, cur.x, cur.y))
   {
      ACS_LocalAmbientSound("player/cbi/buttonpress", 127);
      button->clicked = 5;
   }
   
   return true;
}

cbi_node_t *CBI_ButtonAlloc(int flags, int id, int x, int y, cbi_nodefuncs_t *userfuncs, __str label, __str font)
{
   cbi_button_t *node = calloc(1, sizeof(cbi_button_t));
   
   node->label = label;
   node->font  = font ? font : "CBIFONT";
   
   if(flags & BTNAF_RESPOND_LEFT)
      node->respond += 1;
   if(flags & BTNAF_RESPOND_RIGHT)
      node->respond += 2;
   
   CBI_NodeReset(&node->node, !(flags & NODEAF_NOTVISIBLE), id, x, y, userfuncs);
   node->node.basefuncs.Draw   = CBI_ButtonDraw;
   node->node.basefuncs.Update = CBI_ButtonUpdate;
   node->node.basefuncs.Click  = CBI_ButtonClick;
   
   return &node->node;
}

//
// ---------------------------------------------------------------------------

