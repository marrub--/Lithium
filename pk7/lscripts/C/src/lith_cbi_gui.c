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

cbi_node_t *CBI_NodeListGetByID(dlist_t *list, int id)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      cbi_node_t *node = rover->data.vp;
      
      if(node->id == id)
         return node;
      else if(node->children)
      {
         cbi_node_t *n = CBI_NodeListGetByID(node->children, id);
         if(n)
            return n;
      }
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
   ret++, id--;
   
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
   ret++, id--;
   
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
   
   DrawSpriteAlpha("H_Z3", id, 0.1 + node->x, 0.1 + node->y, TICSECOND, 0.7);
   ret++, id--;
   
   if(button->label)
   {
      char color = 'j';
      
      if(button->clicked)
         color = 'g';
      else if(button->hover)
         color = 'k';
      
      HudMessageF("CBIFONT", "\C%c%S", color, button->label);
      HudMessagePlain(id, node->x + (48 / 2), node->y + (16 / 2), TICSECOND);
      ret++, id--;
   }
   
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
      HudMessagePlain(id - 1, node->x + (48 / 2) + (48 * i), node->y + (14 / 2), TICSECOND);
      DrawSpriteAlpha("H_Z4", id, 0.1 + node->x + (48 * i), 0.1 + node->y, TICSECOND, 0.7);
      
      ret += 2, id -= 2;
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
      if(cur.x < x + 48 && cur.y < node->y + 14 && cur.x >= x && cur.y >= node->y)
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

// ---------------------------------------------------------------------------
// cbi_slider_t
//

float CBI_SliderGetValue(cbi_node_t *node)
{
   cbi_slider_t *slider = (cbi_slider_t *)node;
   return slider->pos * slider->max + slider->min;
}

void CBI_SliderSetPos(cbi_node_t *node, float val)
{
   cbi_slider_t *slider = (cbi_slider_t *)node;
   slider->pos = val;
   ACS_LocalAmbientSound("player/cbi/slidertick", 50);
}

int CBI_SliderDraw(cbi_node_t *node, int id)
{
   cbi_slider_t *slider = (cbi_slider_t *)node;
   cbi_node_t *node = &slider->node;
   int ret = 0;
   
   DrawSpriteAlpha("H_Z5", id, 0.1 + node->x + 2, 0.1 + node->y + 3, TICSECOND, 0.7);
   DrawSpriteAlpha("H_Z6", id - 1, 0.1 + node->x + 3 + (int)(58 * slider->pos), 0.1 + node->y + 1, TICSECOND, 0.7);
   
   if(slider->type == SLDTYPE_INT)
      HudMessageF("CBIFONT", "%i", (int)CBI_SliderGetValue(node));
   else if(slider->type == SLDTYPE_FIXED)
      HudMessageF("CBIFONT", "%.2k", (fixed)CBI_SliderGetValue(node));
   
   HudMessagePlain(id - 2, 0.1 + node->x + 64 + 4, node->y + 4, TICSECOND);
   ret += 3, id -= 3;
   
   if(slider->label)
   {
      HudMessageF("CBIFONT", "\Ct%S", slider->label);
      HudMessageParams(HUDMSG_ALPHA,
         id,
         CR_UNTRANSLATED,
         node->x + (64 / 2),
         node->y + (8 / 2),
         TICSECOND, 0.6);
      ret++, id--;
   }
   
   ret += CBI_NodeDraw(node, id);
   return ret;
}

bool CBI_SliderHold(cbi_node_t *node, player_t *p, struct cursor_s cur, bool left)
{
   cbi_slider_t *slider = (cbi_slider_t *)node;
   cbi_node_t *node = &slider->node;
   
   if(CBI_NodeClick(node, p, cur, left))
      return true;
   
   if(!left)
      return false;
   
   if(cur.x >= node->x     && cur.x <= node->x + 64 &&
      cur.y >= node->y + 2 && cur.y <= node->y + 8)
   {
      float val = minmax(cur.x - (node->x + 2), 0, 60) / 60.0f;
      val = roundf(val * 1000.0f) / 1000.0f;
      
      if(slider->pos != val)
         CBI_SliderSetPos(node, val);
      
      return true;
   }
   
   return false;
}

cbi_node_t *CBI_SliderAlloc(int flags, int id, int x, int y, int type,
   float min, float max, float value, __str label)
{
   cbi_slider_t *node = calloc(1, sizeof(cbi_slider_t));
   
   node->label = label;
   node->type = type;
   node->min = min;
   node->max = max;
   node->pos = normf(value, node->min, node->max);
   node->pos = roundf(node->pos * 1000.0f) / 1000.0f;
   node->node.visible = !(flags & SLDAF_NOTVISIBLE);
   node->node.x = x;
   node->node.y = y;
   node->node.id = id;
   node->node.Draw = CBI_SliderDraw;
   node->node.Update = CBI_NodeUpdate;
   node->node.Click = CBI_NodeClick;
   node->node.Hold = CBI_SliderHold;
   
   return &node->node;
}

// ---------------------------------------------------------------------------
// cbi_checkbox_t
//


int CBI_CheckboxDraw(cbi_node_t *node, int id)
{
   cbi_checkbox_t *checkbox = (cbi_checkbox_t *)node;
   cbi_node_t *node = &checkbox->node;
   int ret = 0;
   
   DrawSpriteAlpha("H_Z7", id, 0.1 + node->x, 0.1 + node->y, TICSECOND, 0.7);
   ret++, id--;
   
   if(checkbox->checked)
   {
      DrawSpritePlain("H_Z8", id, 0.1 + node->x, 0.1 + node->y, TICSECOND);
      ret++, id--;
   }
   
   if(checkbox->label)
   {
      char color = 'j';
      
      if(checkbox->clicked)
         color = 'g';
      else if(checkbox->hover)
         color = 'k';
      
      HudMessageF("CBIFONT", "\C%c%S", color, checkbox->label);
      HudMessagePlain(id, node->x + (checkbox->left ? -12.2 : 12.1), 0.1 + node->y + 1, TICSECOND);
      ret++, id--;
   }
   
   if(checkbox->clicked)
      checkbox->clicked--;
   
   ret += CBI_NodeDraw(node, id);
   return ret;
}

void CBI_CheckboxUpdate(cbi_node_t *node, player_t *p, struct cursor_s cur)
{
   cbi_checkbox_t *checkbox = (cbi_checkbox_t *)node;
   cbi_node_t *node = &checkbox->node;
   
   CBI_NodeUpdate(node, p, cur);
   
   if(cur.x >= node->x + 10 || cur.y >= node->y + 10 ||
      cur.x < node->x || cur.y < node->y)
      checkbox->hover = false;
   else
      checkbox->hover = true;
}

bool CBI_CheckboxClick(cbi_node_t *node, player_t *p, struct cursor_s cur, bool left)
{
   cbi_checkbox_t *checkbox = (cbi_checkbox_t *)node;
   cbi_node_t *node = &checkbox->node;
   
   if(CBI_NodeClick(node, p, cur, left))
      return true;
   
   if(cur.x >= node->x + 10 || cur.y >= node->y + 10 ||
      cur.x < node->x || cur.y < node->y)
      return false;
   
   bool ret = true;
   if(checkbox->Event)
      checkbox->Event(checkbox, p, left, &ret);
   else
      checkbox->checked = !checkbox->checked;
   
   if(ret)
   {
      checkbox->clicked = 5;
      ACS_LocalAmbientSound("player/cbi/buttonpress", 127);
   }
   
   return ret;
}

cbi_node_t *CBI_CheckboxAlloc(int flags, int id, int x, int y, __str label, bool checked, bool left,
   cbi_checkboxevent_t event)
{
   cbi_checkbox_t *node = calloc(1, sizeof(cbi_checkbox_t));
   
   node->checked = checked;
   node->left = left;
   node->label = label;
   node->Event = event;
   node->node.visible = !(flags & CBXAF_NOTVISIBLE);
   node->node.x = x;
   node->node.y = y;
   node->node.id = id;
   node->node.Draw = CBI_CheckboxDraw;
   node->node.Update = CBI_CheckboxUpdate;
   node->node.Click = CBI_CheckboxClick;
   node->node.Hold = CBI_NodeHold;
   
   return &node->node;
}

//
// ---------------------------------------------------------------------------

