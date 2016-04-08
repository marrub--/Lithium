#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_cbi_gui.h"

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

// ---------------------------------------------------------------------------
// Computer-Brain Interface (CBI) Scripts.
//

enum
{
   //
   // uid_base      to uid_base + 99  reserved for Statistics
   // uid_base + 99 to uid_base + 199 reserved for Upgrades
   //
   
   uid_none,
   
   uid_base = 100,
   
   // Statistics
   uid_base_stat = uid_base,
   uid_end_stat  = uid_base + 99,
   
   uid_stat_name = uid_base_stat,
   uid_stat_score_sum,
   uid_stat_score_used,
   uid_stat_health_sum,
   uid_stat_health_used,
   uid_stat_armor_sum,
   uid_stat_armor_used,
   uid_stat_weapons_held,
   uid_stat_secrets_found,
   
   uid_statS,
   uid_statE = uid_stat_score_sum,
   
   // Upgrades
   uid_base_upgrade = uid_base + 100,
   uid_end_upgrade  = uid_base + 199,
   
   uid_upgrade_list = uid_base_upgrade,
};

enum
{
   #define U(en, name) UPGR_##en,
   #include "lith_upgrades.h"
};

static __str upgradenames[] = {
   #define U(en, name) [UPGR_##en] = #en,
   #include "lith_upgrades.h"
};

static
void Menu_Upgrades_DescriptionUpdate(ui_node_t *node, player_t *p, cursor_t cur)
{
   ui_text_t *text = (ui_text_t *)node;
   ui_node_t *node = &text->node;
   
   ui_list_t *list = (ui_list_t *)UI_NodeListGetByID(p->cbi.ui, uid_upgrade_list);
   int i = list ? list->selected : -1;
   
   if(i >= 0)
      text->text = Language("LITH_TXT_UPGRADE_%S", upgradenames[i]);
   
   UI_NodeUpdate(node, p, cur);
}

static
void Menu_Stats_TextUpdate(ui_node_t *node, player_t *p, cursor_t cur)
{
   ui_text_t *text = (ui_text_t *)node;
   ui_node_t *node = &text->node;
   
   switch(node->id)
   {
   case uid_stat_name:          text->text = StrParam("\Cj%S", p->name); break;
   case uid_stat_score_sum:     text->text = StrParam("Score Sum: %lli", p->scoresum); break;
   case uid_stat_score_used:    text->text = StrParam("Score Used: %lli", p->scoreused); break;
   case uid_stat_health_sum:    text->text = StrParam("Health Sum: %li", p->healthsum); break;
   case uid_stat_health_used:   text->text = StrParam("Health Used: %li", p->healthused); break;
   case uid_stat_armor_sum:     text->text = StrParam("Armor Sum: %li", p->armorsum); break;
   case uid_stat_armor_used:    text->text = StrParam("Armor Used: %li", p->armorused); break;
   case uid_stat_weapons_held:  text->text = StrParam("Weapons Held: %i", p->weaponsheld); break;
   case uid_stat_secrets_found: text->text = StrParam("Secrets Found: %i", p->secretsfound); break;
   }
   
   UI_NodeUpdate(node, p, cur);
}

static
bool Menu_Main_XClick(ui_node_t *node, player_t *p, cursor_t cur, bool left)
{
   if(UI_NodeClick(node, p, cur, left))
      return true;
   
   if(!left)
      return false;
   
   if(bpcldi(node->x, node->y, node->x + 11, node->y + 11, cur.x, cur.y))
   {
      Lith_KeyOpenCBI();
      return true;
   }
   
   return false;
}

[[__call("ScriptI")]]
void Lith_PlayerInitCBI(player_t *p)
{
   cbi_t *cbi = &p->cbi;
   
   // Main tab control
   __str tabnames[] = { "Statistics", "Upgrades", "BIP", "Settings", null };
   ui_node_t *tabs = UI_TabAlloc(NODEAF_ALLOCCHILDREN, 0, 13, 13, null, tabnames);
   
   // Statistics
   {
      ui_nodefuncs_t statfunc = { .Update = Menu_Stats_TextUpdate };
      ui_node_t *tab = UI_NodeAlloc(NODEAF_ALLOCCHILDREN);
      
      UI_InsertNode(tab->children, UI_TextAlloc(0, uid_stat_name, 20, 30, &statfunc, null, "SMALLFNT"));
      
      for(int base = uid_statE, i = base; i < uid_statS; i++)
         UI_InsertNode(tab->children, UI_TextAlloc(0, i, 23, 40 + ((i - base) * 8), &statfunc));
      
      UI_InsertNode(tabs->children, tab);
   }
   
   // Upgrades
   {
      ui_nodefuncs_t upgrfunc = { .Update = Menu_Upgrades_DescriptionUpdate };
      ui_node_t *tab = UI_NodeAlloc(NODEAF_ALLOCCHILDREN);
      
      __str listnames[] = {
         #define U(en, name) [UPGR_##en] = name,
         #include "lith_upgrades.h"
         null
      };
      
      UI_InsertNode(tab->children, UI_ListAlloc(0, uid_upgrade_list, 20, 30, null, listnames, 19));
      UI_InsertNode(tab->children, UI_TextAlloc(0, 0, 95, 30, &upgrfunc));
      
      UI_InsertNode(tabs->children, tab);
   }
   
   // BIP (TODO)
   {
      ui_node_t *tab = UI_NodeAlloc(NODEAF_ALLOCCHILDREN);
      UI_InsertNode(tabs->children, tab);
   }
   
   // Main container
   ui_nodefuncs_t xfunc = { .Click = Menu_Main_XClick };
   ui_node_t *ctr = UI_SpriteAlloc(SPRAF_ALPHA | NODEAF_ALLOCCHILDREN, 0, 0, 0, null, "H_Z1", 0.7);
   UI_InsertNode(ctr->children, UI_SpriteAlloc(0, 0, 296, 13, &xfunc, "H_Z9"));
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
      if(ButtonPressedUI(p, BT_ATTACK))
         click = 2;
      else if(ButtonPressedUI(p, BT_ALTATTACK))
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

