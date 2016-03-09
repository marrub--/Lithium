#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"

// ---------------------------------------------------------------------------
// Node Types.
//

typedef int (*cbi_drawfunc_t)(struct cbi_node_s *, int);
typedef void (*cbi_updatefunc_t)(struct cbi_node_s *, struct player_s *, struct cursor_s);
typedef bool (*cbi_clickfunc_t)(struct cbi_node_s *, struct player_s *, struct cursor_s);
typedef void (*cbi_buttonevent_t)(struct cbi_button_s *, struct player_s *) [[__call("ScriptI")]];

typedef struct cbi_node_s
{
   int x, y;
   int id;
   bool visible;
   
   cbi_drawfunc_t Draw;
   cbi_updatefunc_t Update;
   cbi_clickfunc_t Click;
   
   dlist_t *children;
} cbi_node_t;

typedef struct cbi_text_s
{
   cbi_node_t node;
   __str text;
   __str font;
   bool rainbows;
} cbi_text_t;

typedef struct cbi_sprite_s
{
   cbi_node_t node;
   __str name;
   fixed alpha;
} cbi_sprite_t;

typedef struct cbi_button_s
{
   cbi_node_t node;
   __str label;
   bool hover;
   int clicked;
   
   cbi_buttonevent_t Event;
} cbi_button_t;

//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Node Functions.
//

//
// cbi_node_t
//

enum
{
   NAF_NOTVISIBLE = 1 << 0,
};

#define CBI_InsertNode(list, node) \
   DList_InsertBack(list, (listdata_t){ (node) })

//

int CBI_NodeDrawList(dlist_t *list, int id)
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
         Log("Hud ID underflow in CBI_NodeDrawList!");
   }
   
   return ret;
}

void CBI_NodeUpdateList(dlist_t *list, player_t *p, struct cursor_s cur)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      cbi_node_t *node = rover->data.vp;
      
      if(node->visible && node->Update)
         node->Update(node, p, cur);
   }
}

bool CBI_NodeClickList(dlist_t *list, player_t *p, struct cursor_s cur)
{
   for(slist_t *rover = list->head; rover; rover = rover->next)
   {
      cbi_node_t *node = rover->data.vp;
      
      if(node->visible && node->Click)
         if(node->Click(node, p, cur))
         {
            return true;
         }
   }
   
   return false;
}

int CBI_NodeDraw(cbi_node_t *node, int id)
{
   if(node->children)
      return CBI_NodeDrawList(node->children, id);
   
   return 0;
}

void CBI_NodeUpdate(cbi_node_t *node, player_t *p, struct cursor_s cur)
{
   if(node->children)
      CBI_NodeUpdateList(node->children, p, cur);
}

bool CBI_NodeClick(cbi_node_t *node, player_t *p, struct cursor_s cur)
{
   if(node->children)
      return CBI_NodeClickList(node->children, p, cur);
   
   return false;
}

cbi_node_t *CBI_NodeAlloc(int flags, int id, int x, int y)
{
   cbi_node_t *node = calloc(1, sizeof(cbi_node_t));
   
   node->visible = !(flags & NAF_NOTVISIBLE);
   node->x = x;
   node->y = y;
   node->id = id;
   node->Draw = CBI_NodeDraw;
   node->Update = CBI_NodeUpdate;
   node->Click = CBI_NodeClick;
   
   return node;
}

//
// cbi_text_t
//

enum
{
   TAF_NOTVISIBLE = 1 << 0,
   TAF_RAINBOWS   = 1 << 1,
};

int CBI_TextDraw(cbi_node_t *node, int id)
{
   cbi_text_t *text = (cbi_text_t *)node;
   cbi_node_t *node = &text->node;
   int ret;
   
   ret = CBI_NodeDraw(node, id);
   id -= ret;
   
   // The most important check.
   if(text->rainbows)
      HudMessageRainbowsF(text->font, "%S", text->text);
   else
      HudMessageF(text->font, "%S", text->text);
   
   HudMessagePlain(id, 0.1 + node->x, 0.1 + node->y, 0.1);
   
   return ret + 1;
}

[[__optional_args(1)]]
cbi_node_t *CBI_TextAlloc(int flags, int id, int x, int y, __str text, __str font)
{
   cbi_text_t *node = calloc(1, sizeof(cbi_text_t));
   
   node->text = text;
   node->font = font ? font : "CBIFONT";
   node->rainbows = flags & TAF_RAINBOWS;
   node->node.visible = !(flags & TAF_NOTVISIBLE);
   node->node.x = x;
   node->node.y = y;
   node->node.id = id;
   node->node.Draw = CBI_TextDraw;
   node->node.Update = CBI_NodeUpdate;
   node->node.Click = CBI_NodeClick;
   
   return &node->node;
}

//
// cbi_sprite_t
//

enum
{
   SAF_NOTVISIBLE = 1 << 0,
   SAF_ALPHA      = 1 << 1,
};

int CBI_SpriteDraw(cbi_node_t *node, int id)
{
   cbi_sprite_t *sprite = (cbi_sprite_t *)node;
   cbi_node_t *node = &sprite->node;
   int ret;
   
   ret = CBI_NodeDraw(node, id);
   id -= ret;
   
   DrawSprite(sprite->name, HUDMSG_ALPHA, id, 0.1 + node->x, 0.1 + node->y, 0.1, sprite->alpha);
   
   return ret + 1;
}

[[__optional_args(1)]]
cbi_node_t *CBI_SpriteAlloc(int flags, int id, int x, int y, __str name, fixed alpha)
{
   cbi_sprite_t *node = calloc(1, sizeof(cbi_sprite_t));
   
   node->name = name;
   node->alpha = (flags & SAF_ALPHA) ? alpha : 1.0;
   node->node.visible = !(flags & SAF_NOTVISIBLE);
   node->node.x = x;
   node->node.y = y;
   node->node.id = id;
   node->node.Draw = CBI_SpriteDraw;
   node->node.Update = CBI_NodeUpdate;
   node->node.Click = CBI_NodeClick;
   
   return &node->node;
}

//
// cbi_button_t
//

enum
{
   BAF_NOTVISIBLE = 1 << 0,
};

int CBI_ButtonDraw(cbi_node_t *node, int id)
{
   cbi_button_t *button = (cbi_button_t *)node;
   cbi_node_t *node = &button->node;
   int ret;
   int ret;
   
   ret = CBI_NodeDraw(node, id);
   id -= ret;
   
   if(button->label)
   {
      char color = 'j';
      
      if(button->clicked)
         color = 'g';
      else if(button->hover)
         color = 'k';
      
      HudMessageF("CBIFONT", "\C%c%S", color, button->label);
      HudMessagePlain(id - 1, node->x + (48 / 2), node->y + (16 / 2), 0.1);
      ret += 2;
   }
   else
      ret += 1;
   
   DrawSprite("H_Z3", HUDMSG_ALPHA, id, 0.1 + node->x, 0.1 + node->y, 0.1, 0.7);
   
   if(button->clicked)
      button->clicked--;
   
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

bool CBI_ButtonClick(cbi_node_t *node, player_t *p, struct cursor_s cur)
{
   cbi_button_t *button = (cbi_button_t *)node;
   cbi_node_t *node = &button->node;
   
   if(CBI_NodeClick(node, p, cur))
      return true;
   
   if(cur.x >= node->x + 48 || cur.y >= node->y + 16 ||
      cur.x < node->x || cur.y < node->y)
      return false;
   
   button->clicked = 5;
   ACS_LocalAmbientSound("player/cbi/buttonpress", 127);
   
   if(button->Event)
      button->Event(button, p);
   
   return true;
}

[[__optional_args(1)]]
cbi_node_t *CBI_ButtonAlloc(int flags, int id, int x, int y, cbi_buttonevent_t event, __str label)
{
   cbi_button_t *node = calloc(1, sizeof(cbi_button_t));
   
   node->label = label;
   node->Event = event;
   node->node.visible = !(flags & BAF_NOTVISIBLE);
   node->node.x = x;
   node->node.y = y;
   node->node.id = id;
   node->node.Draw = CBI_ButtonDraw;
   node->node.Update = CBI_ButtonUpdate;
   node->node.Click = CBI_ButtonClick;
   
   return &node->node;
}

//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Computer-Brain Interface (CBI) Scripts.
//

[[__call("ScriptI")]]
static
void Button_Generic(cbi_button_t *button, player_t *p)
{
   ACS_Delay(10);
   
   if(button->node.id == 2)
      ACS_Thing_Damage2(0, 9999, "None");
   else if(button->node.id == 1)
      ACS_SetActorProperty(0, APROP_Health, p->health + 9999);
}

[[__call("ScriptI")]]
void Lith_PlayerInitCBI(player_t *p)
{
   register cbi_t *cbi = &p->cbi;
   
   cbi->ui = DList_Create(0);
   CBI_InsertNode(cbi->ui, CBI_SpriteAlloc(SAF_ALPHA, 0, 0, 0, "H_Z1", 0.8));
   CBI_InsertNode(cbi->ui, CBI_TextAlloc(TAF_RAINBOWS, 0, 20, 20, "Comp/Brain OS ver. 1"));
   CBI_InsertNode(cbi->ui, CBI_ButtonAlloc(0, 2, 40, 40, Button_Generic, "Die"));
   CBI_InsertNode(cbi->ui, CBI_ButtonAlloc(0, 1, 40, 60, Button_Generic, "Give Health"));
   CBI_InsertNode(cbi->ui, CBI_TextAlloc(0, 0, 20, 30, "\CjThis is drawn after the buttons"));
   
   cbi->wasinit = true;
}

[[__call("ScriptI")]]
void Lith_PlayerUpdateCBI(player_t *p)
{
   register cbi_t *cbi = &p->cbi;
   
   if(cbi->open)
   {
      cbi->cur.x -= p->yawv * 800.0f;
      
      if(ACS_GetCVar("invertmouse"))
         cbi->cur.y += p->pitchv * 800.0f;
      else
         cbi->cur.y -= p->pitchv * 800.0f;
      
      if(cbi->cur.x < 0) cbi->cur.x = 0;
      if(cbi->cur.y < 0) cbi->cur.y = 0;
      if(cbi->cur.x > 320) cbi->cur.x = 320;
      if(cbi->cur.y > 200) cbi->cur.y = 200;
      
      CBI_NodeUpdateList(cbi->ui, p, cbi->cur);
      
      if(ButtonPressed(p, BT_ATTACK))
         if(!CBI_NodeClickList(cbi->ui, p, cbi->cur))
            ACS_LocalAmbientSound("player/cbi/clickinvalid", 127);
   }
}

[[__call("ScriptI")]]
void Lith_PlayerDrawCBI(player_t *p)
{
   register cbi_t *cbi = &p->cbi;
   
   ACS_SetHudSize(320, 200);
   
   DrawSpritePlain("H_Z2", hid_cbi_cursor, 0.1 + (int)cbi->cur.x, 0.1 + (int)cbi->cur.y, 0.1);
   
   CBI_NodeDrawList(cbi->ui, hid_end_cbi);
}

//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Key Scripts.
//

[[__call("ScriptS"), __extern("ACS")]]
void Lith_KeyOpenCBI()
{
   player_t *p = &players[ACS_PlayerNumber()];
   
   if(p->health < 1)
      return;
   
   p->cbi.open = !p->cbi.open;
   
   if(p->cbi.open)
   {
      p->frozen++;
      ACS_LocalAmbientSound("player/cbi/open", 127);
   }
   else
   {
      p->frozen--;
      ACS_LocalAmbientSound("player/cbi/close", 127);
   }
}

//
// ---------------------------------------------------------------------------

