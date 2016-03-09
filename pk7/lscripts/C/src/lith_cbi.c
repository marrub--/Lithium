#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_cbi_gui.h"

// ---------------------------------------------------------------------------
// Computer-Brain Interface (CBI) Scripts.
//

[[__call("ScriptI")]]
static
void Button_Generic(cbi_button_t *button, player_t *p)
{
   ACS_Delay(10);
   
   switch(button->node.id)
   {
   case 2: ACS_Thing_Damage2(0, 9999, "None"); break;
   case 1: ACS_SetActorProperty(0, APROP_Health, p->health + 9999); break;
   }
}

[[__call("ScriptI")]]
void Lith_PlayerInitCBI(player_t *p)
{
   register cbi_t *cbi = &p->cbi;
   
   cbi->ui = DList_Create();
   
   cbi_node_t *tab = CBI_TabAlloc(0, 0, 10, 10, (__str[]){ "Tab 1", "Empty Tab", null });
   tab->children = DList_Create();
   
   {
      cbi_node_t *tab1 = CBI_NodeAlloc(0, 0, 0, 0);
      tab1->children = DList_Create();
      
      CBI_InsertNode(tab1->children, CBI_TextAlloc(TXTAF_RAINBOWS, 0, 20, 40, "Comp/Brain OS ver. 1"));
      CBI_InsertNode(tab1->children, CBI_ButtonAlloc(0, 2, 40, 60, Button_Generic, "Die"));
      CBI_InsertNode(tab1->children, CBI_ButtonAlloc(0, 1, 40, 80, Button_Generic, "Give Health"));
      CBI_InsertNode(tab1->children, CBI_TextAlloc(0, 0, 20, 50, "\CjThis is drawn after the buttons"));
      
      CBI_InsertNode(tab->children, tab1);
   }
   
   {
      cbi_node_t *tab2 = CBI_NodeAlloc(0, 0, 0, 0);
      tab2->children = DList_Create();
      
      CBI_InsertNode(tab2->children, CBI_TextAlloc(0, 0, 20, 50, "\CjSome text here"));
      
      CBI_InsertNode(tab->children, tab2);
   }
   
   CBI_InsertNode(cbi->ui, CBI_SpriteAlloc(SPRAF_ALPHA, 0, 0, 0, "H_Z1", 0.8));
   CBI_InsertNode(cbi->ui, tab);
   
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
   
   DrawSpritePlain("H_Z2", hid_cbi_cursor, 0.1 + (int)cbi->cur.x, 0.1 + (int)cbi->cur.y, TICSECOND);
   
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

