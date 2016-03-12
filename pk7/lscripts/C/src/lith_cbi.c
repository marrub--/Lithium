#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_cbi_gui.h"

// ---------------------------------------------------------------------------
// Computer-Brain Interface (CBI) Scripts.
//

static
void Menu_TextUpdateFunc(ui_node_t *node, player_t *p, cursor_t cur)
{
   ui_text_t *text = (ui_text_t *)node;
   ui_node_t *node = &text->node;
   
   text->text = StrParam("yay it works %i", ACS_Timer());
}

static
bool Menu_ButtonClickFunc(ui_node_t *node, player_t *p, cursor_t cur, bool left)
{
   ui_button_t *button = (ui_button_t *)node;
   ui_node_t *node = &button->node;
   
   if(left && bpcldi(node->x, node->y, node->x + UI_BUTTON_W, node->y + UI_BUTTON_H, cur.x, cur.y))
      Log("yay it works");
   
   return false;
}

[[__call("ScriptI")]]
void Lith_PlayerInitCBI(player_t *p)
{
   cbi_t *cbi = &p->cbi;
   
   // Main tab control
   ui_node_t *tabs = UI_TabAlloc(NODEAF_ALLOCCHILDREN, 0, 13, 13, null, (__str[]){ "Statistics", "Settings" });
   
   {
      ui_nodefuncs_t textfuncs = { .Update = Menu_TextUpdateFunc };
      ui_nodefuncs_t btnfuncs = { .Click = Menu_ButtonClickFunc };
      ui_node_t *tab = UI_NodeAlloc(NODEAF_ALLOCCHILDREN);
      
      UI_InsertNode(tab->children, UI_ButtonAlloc(BTNAF_RESPOND_LEFT, 0, 50, 50, &btnfuncs, "button"));
      UI_InsertNode(tab->children, UI_TextAlloc(0, 0, 38, 38, &textfuncs, "yay it works"));
      UI_InsertNode(tab->children, UI_TextAlloc(TXTAF_RAINBOWS, 0, 40, 40, &textfuncs, "yay it works"));
      UI_InsertNode(tab->children, UI_TextAlloc(0, 0, 42, 42, &textfuncs, "yay it works"));
      
      UI_InsertNode(tabs->children, tab);
   }
   
   UI_InsertNode(tabs->children, UI_NodeAlloc());
   
   // Main container
   ui_node_t *ctr = UI_SpriteAlloc(SPRAF_ALPHA | NODEAF_ALLOCCHILDREN, 0, 0, 0, null, "H_Z1", 0.7);
   UI_InsertNode(ctr->children, tabs);
   
   // Main list
   cbi->ui = DList_Create();
   UI_InsertNode(cbi->ui, ctr);
   
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
      
      UI_NodeListUpdate(cbi->ui, p, cbi->cur);
      
      int click = 0;
      int hold = 0;
      if(ButtonPressed(p, BT_ATTACK))
         click = 2;
      else if(ButtonPressed(p, BT_ALTATTACK))
         click = 1;
      else if(p->buttons & BT_ATTACK)
         hold = 2;
      else if(p->buttons & BT_ALTATTACK)
         hold = 1;
      
      if(click && !UI_NodeListClick(cbi->ui, p, cbi->cur, click == 2))
         ACS_LocalAmbientSound("player/cbi/clickinvalid", 127);
      else if(hold)
         UI_NodeListHold(cbi->ui, p, cbi->cur, hold == 2);
   }
}

[[__call("ScriptI")]]
void Lith_PlayerDrawCBI(player_t *p)
{
   register cbi_t *cbi = &p->cbi;
   
   ACS_SetHudSize(320, 200);
   
   DrawSpritePlain("H_Z2", hid_cbi_cursor, 0.1 + (int)cbi->cur.x, 0.1 + (int)cbi->cur.y, TICSECOND);
   
   UI_NodeListDraw(cbi->ui, hid_end_cbi);
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

