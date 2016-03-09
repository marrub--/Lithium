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

//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Node Functions.
//

//
// cbi_text_t
//

int CBI_TextDraw(cbi_node_t *node, int id)
{
   cbi_text_t *text = (cbi_text_t *)node;
   
   // The most important check.
   if(text->rainbows)
      HudMessageRainbowsF(text->font, "%S", text->text);
   else
      HudMessageF(text->font, "%S", text->text);
   
   HudMessagePlain(id, 0.1 + text->node.x, 0.1 + text->node.y, 0.1);
   
   return 1;
}

[[__optional_args(2)]]
cbi_node_t *CBI_TextAlloc(__str text, __str font, bool rainbows)
{
   cbi_text_t *node = calloc(1, sizeof(cbi_text_t));
   
   node->text = text;
   node->font = font ? font : "SMALLFNT";
   node->node.visible = true;
   node->node.Draw = CBI_TextDraw;
   
   return &node->node;
}

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
   DList_InsertBack(cbi->ui, (listdata_t){ CBI_TextAlloc("yay it works") });
   
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
      int id = hid_base_cbi;
      
      ACS_SetHudSize(320, 200);
      for(slist_t *rover = cbi->ui->head; rover; rover = rover->next)
      {
         cbi_node_t *node = rover->data.vp;
         
         if(node->visible && node->Draw)
            id += node->Draw(node, id);
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
      ACS_LocalAmbientSound("player/cbi/open", 127);
   else
      ACS_LocalAmbientSound("player/cbi/close", 127);
}

//
// ---------------------------------------------------------------------------

