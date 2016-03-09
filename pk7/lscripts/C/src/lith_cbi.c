#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"

// ---------------------------------------------------------------------------
// Node Types.
//

typedef int (*cbi_func_t)(struct cbi_node_s *, int);

typedef struct cbi_node_s
{
   int x, y;
   bool visible;
   
   cbi_func_t Draw;
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

//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Node Functions.
//

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
   
   // The most important check.
   if(text->rainbows)
      HudMessageRainbowsF(text->font, "%S", text->text);
   else
      HudMessageF(text->font, "%S", text->text);
   
   HudMessagePlain(id, 0.1 + node->x, 0.1 + node->y, 0.1);
   
   return 1;
}

[[__optional_args(1)]]
cbi_node_t *CBI_TextAlloc(int flags, int x, int y, __str text, __str font)
{
   cbi_text_t *node = calloc(1, sizeof(cbi_text_t));
   
   node->text = text;
   node->font = font ? font : "CBIFONT";
   node->rainbows = flags & TAF_RAINBOWS;
   node->node.visible = !(flags & TAF_NOTVISIBLE);
   node->node.x = x;
   node->node.y = y;
   node->node.Draw = CBI_TextDraw;
   
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
   DrawSprite(sprite->name, HUDMSG_ALPHA, id, 0.1 + node->x, 0.1 + node->y, 0.1, sprite->alpha);
   return 1;
}

[[__optional_args(1)]]
cbi_node_t *CBI_SpriteAlloc(int flags, int x, int y, __str name, fixed alpha)
{
   cbi_sprite_t *node = calloc(1, sizeof(cbi_sprite_t));
   
   node->name = name;
   node->alpha = (flags & SAF_ALPHA) ? alpha : 1.0;
   node->node.visible = !(flags & SAF_NOTVISIBLE);
   node->node.x = x;
   node->node.y = y;
   node->node.Draw = CBI_SpriteDraw;
   
   return &node->node;
}

//
// cbi_node_t
//

#define CBI_InsertNode(cbi, node) \
   DList_InsertBack((cbi)->ui, (listdata_t){ (node) })

//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Computer-Brain Interface (CBI) Scripts.
//

[[__call("ScriptI")]]
void Lith_PlayerInitCBI(player_t *p)
{
   register cbi_t *cbi = &p->cbi;
   
   cbi->ui = DList_Create(0);
   CBI_InsertNode(cbi, CBI_SpriteAlloc(SAF_ALPHA, 0, 0, "H_Z1", 0.8));
   CBI_InsertNode(cbi, CBI_TextAlloc(0, 20, 20, "\CjComp/Brain OS ver. 1"));
   
   cbi->wasinit = true;
}

[[__call("ScriptI")]]
void Lith_PlayerUpdateCBI(player_t *p)
{
}

[[__call("ScriptI")]]
void Lith_PlayerDrawCBI(player_t *p)
{
   register cbi_t *cbi = &p->cbi;
   
   if(cbi->open)
   {
      int id = hid_end_cbi;
      
      ACS_SetHudSize(320, 200);
      for(slist_t *rover = cbi->ui->head; rover; rover = rover->next)
      {
         cbi_node_t *node = rover->data.vp;
         
         if(node->visible && node->Draw)
            id -= node->Draw(node, id);
         
         if(id < hid_cbi_underflow)
            Log("Hud ID underflow in Lith_PlayerDrawCBI!");
      }
   }
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

