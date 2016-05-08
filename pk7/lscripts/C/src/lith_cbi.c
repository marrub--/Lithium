#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_cbi_gui.h"

// ---------------------------------------------------------------------------
// Key Scripts.
//

[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyOpenCBI(void)
{
   player_t *p = &players[ACS_PlayerNumber()];
   
   if(p->dead)
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
   uid_upgrade_own,
   uid_upgrade_active
};

// ---------------------------------------------------------------------------
// Upgrades.
//

//
// Buy button.
//

static
void Menu_Upgrades_BuyClick(ui_node_t *node, player_t *p, cursor_t *cur)
{
   ui_list_t *list = (ui_list_t *)UI_NodeListGetByID(node->parent->children, uid_upgrade_list);
   if(!list)
      return;
   upgrade_t *upgr = &p->upgrades[list->selected];
   
   Upgr_Buy(p, upgr);
}

static
void Menu_Upgrades_BuyUpdate(ui_node_t *node, player_t *p, cursor_t *cur)
{
   ui_button_t *button = (ui_button_t *)node;
   ui_node_t *node = &button->node;
   
   ui_list_t *list = (ui_list_t *)UI_NodeListGetByID(node->parent->children, uid_upgrade_list);
   if(!list)
      return;
   upgrade_t *upgr = &p->upgrades[list->selected];
   
   button->active = Upgr_CanBuy(p, upgr);
}

//
// Activate button.
//

static
void Menu_Upgrades_ActivateClick(ui_node_t *node, player_t *p, cursor_t *cur)
{
   ui_list_t *list = (ui_list_t *)UI_NodeListGetByID(node->parent->children, uid_upgrade_list);
   if(!list)
      return;
   upgrade_t *upgr = &p->upgrades[list->selected];
   
   Upgr_ToggleActive(p, upgr);
}

static
void Menu_Upgrades_ActivateUpdate(ui_node_t *node, player_t *p, cursor_t *cur)
{
   ui_button_t *button = (ui_button_t *)node;
   ui_node_t *node = &button->node;
   
   ui_list_t *list = (ui_list_t *)UI_NodeListGetByID(node->parent->children, uid_upgrade_list);
   if(!list)
      return;
   upgrade_t *upgr = &p->upgrades[list->selected];
   
   button->active = upgr->owned;
   button->label = upgr->active ? "Deactivate" : "Activate";
}

//
// 'Active' text.
//

static
void Menu_Upgrades_ActiveUpdate(ui_node_t *node, player_t *p, cursor_t *cur)
{
   ui_text_t *text = (ui_text_t *)node;
   ui_node_t *node = &text->node;
   
   ui_list_t *list = (ui_list_t *)UI_NodeListGetByID(node->parent->children, uid_upgrade_list);
   if(!list)
      return;
   upgrade_t *upgr = &p->upgrades[list->selected];
   
   if(node->id == uid_upgrade_own)
      node->nodraw = !upgr->owned;
   else if(node->id == uid_upgrade_active)
      node->nodraw = !upgr->active;
}

//
// Description.
//

static
void Menu_Upgrades_DescriptionUpdate(ui_node_t *node, player_t *p, cursor_t *cur)
{
   ui_text_t *text = (ui_text_t *)node;
   ui_node_t *node = &text->node;
   
   ui_list_t *list = (ui_list_t *)UI_NodeListGetByID(node->parent->children, uid_upgrade_list);
   if(!list)
      return;
   upgrade_t *upgr = &p->upgrades[list->selected];
   
   text->text = upgr->description;
}

static
int Menu_Upgrades_DescriptionPreDraw(ui_node_t *node, int id)
{
   ACS_SetHudClipRect(node->x, node->y, 200, 150, 200);
   return 0;
}

static
int Menu_Upgrades_DescriptionPostDraw(ui_node_t *node, int id)
{
   ACS_SetHudClipRect(0, 0, 0, 0);
   return 0;
}

// ---------------------------------------------------------------------------
// Statistics.
//

//
// Main text script.
//

static
void Menu_Stats_TextUpdate(ui_node_t *node, player_t *p, cursor_t *cur)
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
}

// ---------------------------------------------------------------------------
// Main menu.
//

static
void Menu_Main_XUpdate(ui_node_t *node, player_t *p, cursor_t *cur)
{
   if(cur->click & CLICK_LEFT && bpcldi(node->x, node->y, node->x + UI_XBUTTON_W, node->y + UI_XBUTTON_H, cur->x, cur->y))
      Lith_KeyOpenCBI();
}

// ---------------------------------------------------------------------------
// Computer-Brain Interface (CBI) Scripts.
//

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
      
      UI_InsertNode(tab, UI_TextAlloc(0, uid_stat_name, 20, 30, &statfunc, null, "SMALLFNT"));
      
      for(int base = uid_statE, i = base; i < uid_statS; i++)
         UI_InsertNode(tab, UI_TextAlloc(0, i, 23, 40 + ((i - base) * 8), &statfunc));
      
      UI_InsertNode(tabs, tab);
   }
   
   // Upgrades
   {
      ui_nodefuncs_t upgrfunc = {
         .Update = Menu_Upgrades_DescriptionUpdate,
         .PreDraw = Menu_Upgrades_DescriptionPreDraw,
         .PostDraw = Menu_Upgrades_DescriptionPostDraw
      };
      ui_nodefuncs_t buyfunc = {
         .Click = Menu_Upgrades_BuyClick,
         .Update = Menu_Upgrades_BuyUpdate
      };
      ui_nodefuncs_t activefunc = { .Update = Menu_Upgrades_ActiveUpdate };
      ui_nodefuncs_t activatefunc = {
         .Click = Menu_Upgrades_ActivateClick,
         .Update = Menu_Upgrades_ActivateUpdate
      };
      ui_node_t *tab = UI_NodeAlloc(NODEAF_ALLOCCHILDREN);
      
      UI_InsertNode(tab, UI_ListAlloc(0, uid_upgrade_list, 20, 30, null, upgrade_names, 19));
      UI_InsertNode(tab, UI_TextAlloc(0, 0, 95, 30, &upgrfunc));
      UI_InsertNode(tab, UI_TextAlloc(0, uid_upgrade_own, 209, 159, &activefunc, "You own this upgrade."));
      UI_InsertNode(tab, UI_TextAlloc(0, uid_upgrade_active, 207, 151, &activefunc, "This upgrade is active."));
      UI_InsertNode(tab, UI_ButtonAlloc(0, 0, 259, 170, &buyfunc, "Buy"));
      UI_InsertNode(tab, UI_ButtonAlloc(0, 0, 209, 170, &activatefunc));
      
      UI_InsertNode(tabs, tab);
   }
   
   // TODO: BIP
   {
      ui_node_t *tab = UI_NodeAlloc(NODEAF_ALLOCCHILDREN);
      UI_InsertNode(tabs, tab);
   }
   
   // Main container
   ui_nodefuncs_t xfunc = { .Update = Menu_Main_XUpdate };
   ui_node_t *ctr = UI_SpriteAlloc(SPRAF_ALPHA | NODEAF_ALLOCCHILDREN, 0, 0, 0, null, "lgfx/UI/Background.png", 0.7);
   UI_InsertNode(ctr, UI_SpriteAlloc(0, 0, 296, 13, &xfunc, "lgfx/UI/ExitButton.png"));
   UI_InsertNode(ctr, tabs);
   
   // Main list
   cbi->ui = DList_Create();
   DList_InsertBack(cbi->ui, (listdata_t){ ctr });
   
   cbi->wasinit = true;
}

[[__call("ScriptI")]]
void Lith_PlayerUpdateCBI(player_t *p)
{
   cbi_t *cbi = &p->cbi;
   
   if(cbi->open)
   {
      cbi->cur.x -= p->yawv * 800.0f;
      
      if(ACS_GetCVar("invertmouse"))
         cbi->cur.y += p->pitchv * 800.0f;
      else
         cbi->cur.y -= p->pitchv * 800.0f;
      
      if(cbi->cur.x < 0)
         cbi->cur.x = 0;
      if(cbi->cur.y < 0)
         cbi->cur.y = 0;
      if(cbi->cur.x > 320)
         cbi->cur.x = 320;
      if(cbi->cur.y > 200)
         cbi->cur.y = 200;
      
      cbi->cur.click = CLICK_NONE;
      cbi->cur.hold = CLICK_NONE;
      
      if(ButtonPressedUI(p, BT_ATTACK))
         cbi->cur.click |= CLICK_LEFT;
      else if(p->buttons & BT_ATTACK)
         cbi->cur.hold |= CLICK_LEFT;
      
      if(ButtonPressedUI(p, BT_ALTATTACK))
         cbi->cur.click |= CLICK_RIGHT;
      else if(p->buttons & BT_ALTATTACK)
         cbi->cur.hold |= CLICK_RIGHT;
      
      UI_NodeListUpdate(cbi->ui, p, &cbi->cur);
   }
}

[[__call("ScriptI")]]
void Lith_PlayerDrawCBI(player_t *p)
{
   cbi_t *cbi = &p->cbi;
   
   ACS_SetHudSize(320, 200);
   
   DrawSpritePlain("lgfx/UI/Cursor.png", hid_cbi_cursor,
                   0.1 + (int)cbi->cur.x, 0.1 + (int)cbi->cur.y, TICSECOND);
   
   UI_NodeListDraw(cbi->ui, hid_end_cbi);
}

//
// ---------------------------------------------------------------------------

