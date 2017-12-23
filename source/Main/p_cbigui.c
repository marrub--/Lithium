// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
// vim: columns=120
#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_world.h"

// Static Functions ----------------------------------------------------------|

//
// Lith_CBITab_Arsenal
//
static void Lith_CBITab_Arsenal(gui_state_t *g, player_t *p)
{
   extern void Lith_CBITab_Upgrades(gui_state_t *g, player_t *p);
   extern void Lith_CBITab_Shop    (gui_state_t *g, player_t *p);

   static __str const tabnames[] = {"Upgrades", "Shop"};
   Lith_GUI_Tabs(g, st_arsetab, tabnames, 13, 13, 1);

   switch(g->st[st_arsetab].i) {
   case cbi_tab_arsenal_upgrades: Lith_CBITab_Upgrades(g, p); break;
   case cbi_tab_arsenal_shop:     Lith_CBITab_Shop    (g, p); break;
   }
}

//
// Lith_CBITab_Stat
//
static void Lith_CBITab_Stat(gui_state_t *g, player_t *p)
{
   extern void Lith_CBITab_CBI   (gui_state_t *g, player_t *p);
   extern void Lith_CBITab_Status(gui_state_t *g, player_t *p);

   static __str const tabnames[] = {"Attributes", "CBI"};
   Lith_GUI_Tabs(g, st_stattab, tabnames, 13, 13, 1);

   switch(g->st[st_stattab].i) {
   case cbi_tab_stat_attr: Lith_CBITab_Status(g, p); break;
   case cbi_tab_stat_cbi:  Lith_CBITab_CBI   (g, p); break;
   }
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_PlayerUpdateCBIGUI
//
[[__call("ScriptS")]]
void Lith_PlayerUpdateCBIGUI(player_t *p)
{
   gui_state_t *g = &p->cbi.guistate;

   p->cbi.theme = p->getCVarI("lith_gui_theme");

   if(p->cbi.theme != p->cbi.oldtheme)
   {
      switch((p->cbi.oldtheme = p->cbi.theme))
      {
      default:
      case cbi_theme_green:    p->cbi.guistate.gfxprefix = "lgfx/UI_Green/";    break;
      case cbi_theme_rose:     p->cbi.guistate.gfxprefix = "lgfx/UI_Rose/";     break;
      case cbi_theme_umi:      p->cbi.guistate.gfxprefix = "lgfx/UI_Umi/";      break;
      case cbi_theme_ender:    p->cbi.guistate.gfxprefix = "lgfx/UI_Ender/";    break;
      case cbi_theme_orange:   p->cbi.guistate.gfxprefix = "lgfx/UI_Orange/";   break;
      case cbi_theme_grey:     p->cbi.guistate.gfxprefix = "lgfx/UI_Grey/";     break;
      case cbi_theme_bassilla: p->cbi.guistate.gfxprefix = "lgfx/UI_Bassilla/"; break;
      case cbi_theme_ghost:    p->cbi.guistate.gfxprefix = "lgfx/UI_Ghost/";    break;
      case cbi_theme_winxp:    p->cbi.guistate.gfxprefix = "lgfx/UI_WinXP/";    break;
      }
   }

   Lith_GUI_Begin(g, hid_end_cbi, 320, 240);

   if(!p->indialogue)
      Lith_GUI_UpdateState(g, p);

   DrawSpriteAlpha(StrParam("%SBackground.png", g->gfxprefix), g->hid--, 0.1, 0.1, TICSECOND, 0.7);

   if(Lith_GUI_Button(g, .x = 296, 13, .preset = &guipre.btnexit))
      p->useGUI(GUI_CBI);

   static __str tabnames[] = {"", "Status", "Info", "Settings"};
   tabnames[0] = Language("LITH_TXT_Arsenal%S", p->discrim);
   Lith_GUI_Tabs(g, st_maintab, tabnames, 13, 13, 0);

   extern void Lith_CBITab_BIP     (gui_state_t *g, player_t *p);
   extern void Lith_CBITab_Settings(gui_state_t *g, player_t *p);

   switch(g->st[st_maintab].i)
   {
   case cbi_tab_arsenal:  Lith_CBITab_Arsenal (g, p); break;
   case cbi_tab_status:   Lith_CBITab_Stat    (g, p); break;
   case cbi_tab_bip:      Lith_CBITab_BIP     (g, p); break;
   case cbi_tab_settings: Lith_CBITab_Settings(g, p); break;
   }

   Lith_GUI_End(g);
}

//
// Lith_PlayerResetCBIGUI
//
void Lith_PlayerResetCBIGUI(player_t *p)
{
   p->cbi.guistate.cx = 320 / 2;
   p->cbi.guistate.cy = 240 / 2;

   Lith_GUI_Init(&p->cbi.guistate, st_max);

   p->cbi.guistate.st[st_upgrselold].i = -1;

   p->cbi.oldtheme = -1;
}

// Scripts -------------------------------------------------------------------|

//
// Lith_KeyOpenCBI
//
[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyOpenCBI(void)
{
   if(ACS_Timer() < 10) return;

   withplayer(LocalPlayer) {
      p->bip.curcategory = BIPC_MAIN;
      p->bip.curpage = null;
      p->useGUI(GUI_CBI);
   }
}

// EOF

