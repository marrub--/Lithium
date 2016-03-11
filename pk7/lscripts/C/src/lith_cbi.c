#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_cbi_gui.h"

// ---------------------------------------------------------------------------
// Computer-Brain Interface (CBI) Scripts.
//

enum
{
   uid_none,
   uid_stats_name,
   uid_stats_health,
   uid_stats_armor,
   uid_stats_score,
   uid_stats_scoresum,
   uid_stats_scoreused,
   uid_stats_end,
   
   uid_stats_start = uid_stats_health,
};

[[__call("ScriptI")]]
static
void Menu_Stats_GenericText(cbi_text_t *text, player_t *p)
{
   cbi_node_t *node = &text->node;
   
   switch(node->id)
   {
   case uid_stats_name:      text->text = StrParam("\Cj%S", p->name); break;
   case uid_stats_health:    text->text = StrParam("Health: %i", p->health); break;
   case uid_stats_armor:     text->text = StrParam("Armor: %i", p->armor); break;
   case uid_stats_score:     text->text = StrParam("Score: %lli", p->score); break;
   case uid_stats_scoresum:  text->text = StrParam("Score Sum: %lli", p->scoresum); break;
   case uid_stats_scoreused: text->text = StrParam("Score Used: %lli", p->scoreused); break;
   }
}

static
cbi_node_t *Menu_InitStatistics()
{
   cbi_node_t *tab = CBI_NodeAlloc();
   tab->children = DList_Create();
   
   CBI_InsertNode(tab->children,
      CBI_TextAlloc(0, uid_stats_name, 20, 30, null, "SMALLFNT", Menu_Stats_GenericText));
   
   for(int i = uid_stats_start; i < uid_stats_end; i++)
   {
      int ofs = 8 * (i - uid_stats_start);
      CBI_InsertNode(tab->children, CBI_TextAlloc(0, i, 25, 40 + ofs, null, null, Menu_Stats_GenericText));
   }
   
   return tab;
}

[[__call("ScriptI")]]
void Lith_PlayerInitCBI(player_t *p)
{
   cbi_t *cbi = &p->cbi;
   
   // Tab control
   __str tabnames[] = {
      "Statistics",
      "Upgrades",
      "BIP",
      "Settings",
      null
   };
   
   cbi_node_t *tabc = CBI_TabAlloc(0, 0, 13, 13, tabnames);
   tabc->children = DList_Create();
   
   CBI_InsertNode(tabc->children, Menu_InitStatistics());
   CBI_InsertNode(tabc->children, CBI_NodeAlloc());
   CBI_InsertNode(tabc->children, CBI_NodeAlloc());
   CBI_InsertNode(tabc->children, CBI_NodeAlloc());
   //CBI_InsertNode(tabc->children, Menu_InitUpgrades());
   //CBI_InsertNode(tabc->children, Menu_InitBIP());
   //CBI_InsertNode(tabc->children, Menu_InitSettings());
   
   // Container
   cbi_node_t *container = CBI_SpriteAlloc(SPRAF_ALPHA, 0, 0, 0, "H_Z1", 0.8);
   container->children = DList_Create();
   
   CBI_InsertNode(container->children, CBI_TextAlloc(TXTAF_RAINBOWS, 0, 20, 175, "Comp/Brain OS ver. 1"));
   CBI_InsertNode(container->children, tabc);
   
   // Main list
   cbi->ui = DList_Create();
   CBI_InsertNode(cbi->ui, container);
   
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
      
      CBI_NodeListUpdate(cbi->ui, p, cbi->cur);
      
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
      
      if(click && !CBI_NodeListClick(cbi->ui, p, cbi->cur, click == 2))
         ACS_LocalAmbientSound("player/cbi/clickinvalid", 127);
      else if(hold)
         CBI_NodeListHold(cbi->ui, p, cbi->cur, hold == 2);
   }
}

[[__call("ScriptI")]]
void Lith_PlayerDrawCBI(player_t *p)
{
   register cbi_t *cbi = &p->cbi;
   
   ACS_SetHudSize(320, 200);
   
   DrawSpritePlain("H_Z2", hid_cbi_cursor, 0.1 + (int)cbi->cur.x, 0.1 + (int)cbi->cur.y, TICSECOND);
   
   CBI_NodeListDraw(cbi->ui, hid_end_cbi);
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

