#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_cbi_gui.h"
#include <math.h>

// ---------------------------------------------------------------------------
// Node Functions.
//

int UI_NodeListDraw(dlist_t *list, int id)
{
   int ret = 0;
   
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      ui_node_t *node = rover->data.vp;
      
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
         Log("Hud ID underflow in UI_NodeListDraw!");
   }
   
   return ret;
}

void UI_NodeListUpdate(dlist_t *list, player_t *p, cursor_t cur)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      ui_node_t *node = rover->data.vp;
      
      if(node->visible)
      {
         node->basefuncs.Update(node, p, cur);
         
         if(node->userfuncs.Update)
            node->userfuncs.Update(node, p, cur);
      }
   }
}

bool UI_NodeListClick(dlist_t *list, player_t *p, cursor_t cur, bool left)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      ui_node_t *node = rover->data.vp;
      
      if(node->visible)
         if((node->userfuncs.Click && node->userfuncs.Click(node, p, cur, left)) ||
            node->basefuncs.Click(node, p, cur, left))
            return true;
   }
   
   return false;
}

bool UI_NodeListHold(dlist_t *list, player_t *p, cursor_t cur, bool left)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      ui_node_t *node = rover->data.vp;
      
      if(node->visible)
         if((node->userfuncs.Hold && node->userfuncs.Hold(node, p, cur, left)) ||
            node->basefuncs.Hold(node, p, cur, left))
            return true;
   }
   
   return false;
}

ui_node_t *UI_NodeListGetByID(dlist_t *list, int id)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      ui_node_t *node = rover->data.vp;
      ui_node_t *cnode;
      
      if(node->id == id)
         return node;
      else if(node->children)
         if((cnode = UI_NodeListGetByID(node->children, id)))
            return cnode;
   }
   
   return null;
}

// ---------------------------------------------------------------------------
// ui_node_t
//

int UI_NodeDraw(ui_node_t *node, int id)
{
   if(node->children)
      return UI_NodeListDraw(node->children, id);
   
   return 0;
}

void UI_NodeUpdate(ui_node_t *node, player_t *p, cursor_t cur)
{
   if(node->children)
      UI_NodeListUpdate(node->children, p, cur);
}

bool UI_NodeClick(ui_node_t *node, player_t *p, cursor_t cur, bool left)
{
   if(node->children)
      return UI_NodeListClick(node->children, p, cur, left);
   
   return false;
}

bool UI_NodeHold(ui_node_t *node, player_t *p, cursor_t cur, bool left)
{
   if(node->children)
      return UI_NodeListHold(node->children, p, cur, left);
   
   return false;
}

void UI_NodeReset(ui_node_t *node, int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs)
{
   flags = flags & 0x1F; // We only care about the first 5 bits.
   
   node->visible = !(flags & NODEAF_NOTVISIBLE);
   node->id = id;
   node->x = x;
   node->y = y;
   node->basefuncs.Draw   = UI_NodeDraw;
   node->basefuncs.Update = UI_NodeUpdate;
   node->basefuncs.Click  = UI_NodeClick;
   node->basefuncs.Hold   = UI_NodeHold;
   
   if(flags & NODEAF_ALLOCCHILDREN)
      node->children = DList_Create();
   
   if(userfuncs)
      memcpy(&node->userfuncs, userfuncs, sizeof(ui_nodefuncs_t));
   else
      memset(&node->userfuncs, 0, sizeof(ui_nodefuncs_t));
}

ui_node_t *UI_NodeAlloc(int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs)
{
   ui_node_t *node = calloc(1, sizeof(ui_node_t));
   UI_NodeReset(node, flags, id, x, y, userfuncs);
   return node;
}

// ---------------------------------------------------------------------------
// ui_text_t
//

int UI_TextDraw(ui_node_t *node, int id)
{
   ui_text_t *text = (ui_text_t *)node;
   ui_node_t *node = &text->node;
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
   
   ret += UI_NodeDraw(node, id);
   return ret;
}

ui_node_t *UI_TextAlloc(int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs, __str text, __str font)
{
   ui_text_t *node = calloc(1, sizeof(ui_text_t));
   
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
   
   UI_NodeReset(&node->node, flags, id, x, y, userfuncs);
   node->node.basefuncs.Draw = UI_TextDraw;
   
   return &node->node;
}

// ---------------------------------------------------------------------------
// ui_sprite_t
//

int UI_SpriteDraw(ui_node_t *node, int id)
{
   ui_sprite_t *sprite = (ui_sprite_t *)node;
   ui_node_t *node = &sprite->node;
   int ret = 0;
   
   if(sprite->name)
   {
      DrawSpriteAlpha(sprite->name, id,
         sprite->alignx + node->x,
         sprite->aligny + node->y,
         TICSECOND, sprite->alpha);
      id--, ret++;
   }
   
   ret += UI_NodeDraw(node, id);
   return ret;
}

ui_node_t *UI_SpriteAlloc(int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs, __str name, fixed alpha)
{
   ui_sprite_t *node = calloc(1, sizeof(ui_sprite_t));
   
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
   
   UI_NodeReset(&node->node, flags, id, x, y, userfuncs);
   node->node.basefuncs.Draw = UI_SpriteDraw;
   
   return &node->node;
}

// ---------------------------------------------------------------------------
// ui_button_t
//

int UI_ButtonDraw(ui_node_t *node, int id)
{
   ui_button_t *button = (ui_button_t *)node;
   ui_node_t *node = &button->node;
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
      HudMessagePlain(id, (UI_BUTTON_W / 2) + node->x, (UI_BUTTON_H / 2) + node->y, TICSECOND);
      
      id--, ret++;
   }
   
   ret += UI_NodeDraw(node, id);
   return ret;
}

void UI_ButtonUpdate(ui_node_t *node, player_t *p, cursor_t cur)
{
   ui_button_t *button = (ui_button_t *)node;
   ui_node_t *node = &button->node;
   
   if(button->clicked < 0)
      button->clicked = 0;
   else if(button->clicked > 0)
      button->clicked--;
   
   if(bpcldi(node->x, node->y, node->x + UI_BUTTON_W, node->y + UI_BUTTON_H, cur.x, cur.y) && button->clicked == 0)
      button->clicked = -1;
   
   UI_NodeUpdate(node, p, cur);
}

bool UI_ButtonClick(ui_node_t *node, player_t *p, cursor_t cur, bool left)
{
   ui_button_t *button = (ui_button_t *)node;
   ui_node_t *node = &button->node;
   
   if(UI_NodeClick(node, p, cur, left))
      return true;
   
   if((left  && !(button->respond == 1 || button->respond == 3)) ||
      (!left && !(button->respond == 2 || button->respond == 3)))
      return false;
   
   if(bpcldi(node->x, node->y, node->x + UI_BUTTON_W, node->y + UI_BUTTON_H, cur.x, cur.y))
   {
      ACS_LocalAmbientSound("player/cbi/buttonpress", 127);
      button->clicked = 5;
      return true;
   }
   
   return false;
}

ui_node_t *UI_ButtonAlloc(int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs, __str label, __str font)
{
   ui_button_t *node = calloc(1, sizeof(ui_button_t));
   
   node->label = label;
   node->font  = font ? font : "CBIFONT";
   
   if(flags & BTNAF_RESPOND_LEFT)
      node->respond += 1;
   if(flags & BTNAF_RESPOND_RIGHT)
      node->respond += 2;
   
   UI_NodeReset(&node->node, flags, id, x, y, userfuncs);
   node->node.basefuncs.Draw   = UI_ButtonDraw;
   node->node.basefuncs.Update = UI_ButtonUpdate;
   node->node.basefuncs.Click  = UI_ButtonClick;
   
   return &node->node;
}

// ---------------------------------------------------------------------------
// ui_tab_t
//

int UI_TabDraw(ui_node_t *node, int id)
{
   ui_tab_t *tab = (ui_tab_t *)node;
   ui_node_t *node = &tab->node;
   int ret = 0;
   
   if(tab->names)
   {
      for(int i = 0; i < tab->ntabs; i++)
      {
         char color = 'j';
         
         if(tab->clicked && i == tab->curtab)
            color = 'g';
         else if(i == tab->curtab)
            color = 'n';
         else if(i == tab->hover)
            color = 'f';
         
         DrawSpriteAlpha("H_Z4", id, 0.1 + node->x + (UI_TAB_W * i), 0.1 + node->y, TICSECOND, 0.7);
         id--, ret++;
         
         HudMessageF("CBIFONT", "\C%c%S", color, tab->names[i]);
         HudMessagePlain(id, 
            0.0 + node->x + (UI_TAB_W * i) + (UI_TAB_W / 2),
            0.0 + node->y + (UI_TAB_H / 2),
            TICSECOND);
         id--, ret++;
      }
   }
   
   ret += UI_NodeDraw(node, id);
   return ret;
}

void UI_TabUpdate(ui_node_t *node, player_t *p, cursor_t cur)
{
   ui_tab_t *tab = (ui_tab_t *)node;
   ui_node_t *node = &tab->node;
   
   tab->hover = -1;
   
   if(tab->clicked > 0)
      tab->clicked--;
   
   {
      int i = 0;
      for(slist_t *rover = node->children->head; rover; rover = rover->next, i++)
      {
         ui_node_t *node = rover->data.vp;
         if(i == tab->curtab)
            node->visible = true;
         else
            node->visible = false;
      }
   }
   
   for(int i = 0; i < tab->ntabs; i++)
   {
      if(tab->clicked && i == tab->curtab)
         continue;
      
      int x = node->x + (UI_TAB_W * i);
      if(bpcldi(x, node->y, x + UI_TAB_W, node->y + UI_TAB_H, cur.x, cur.y))
      {
         tab->hover = i;
         break;
      }
   }
   
   UI_NodeUpdate(node, p, cur);
}

bool UI_TabClick(ui_node_t *node, player_t *p, cursor_t cur, bool left)
{
   ui_tab_t *tab = (ui_tab_t *)node;
   ui_node_t *node = &tab->node;
   
   if(UI_NodeClick(node, p, cur, left))
      return true;
   
   if(!left)
      return false;
   
   for(int i = 0; i < tab->ntabs; i++)
   {
      if(i == tab->curtab)
         continue;
      
      int x = node->x + (UI_TAB_W * i);
      if(bpcldi(x, node->y, x + UI_TAB_W, node->y + UI_TAB_H, cur.x, cur.y))
      {
         ACS_LocalAmbientSound("player/cbi/buttonpress", 127);
         tab->clicked = 5;
         tab->curtab = i;
         return true;
      }
   }
   
   return false;
}

ui_node_t *UI_TabAlloc(int flags, int id, int x, int y, ui_nodefuncs_t *userfuncs, __str *names)
{
   ui_tab_t *node = calloc(1, sizeof(ui_tab_t));
   
   if(names)
   {
      int ntabs = 0;
      
      for(__str *p = names; *p; p++)
         ntabs++;
      
      node->ntabs = ntabs;
      node->names = cpyalloc(ntabs, sizeof(__str), names);
   }
   
   UI_NodeReset(&node->node, flags, id, x, y, userfuncs);
   node->node.basefuncs.Draw   = UI_TabDraw;
   node->node.basefuncs.Update = UI_TabUpdate;
   node->node.basefuncs.Click  = UI_TabClick;
   
   return &node->node;
}


//
// ---------------------------------------------------------------------------

