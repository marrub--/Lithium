// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_list.h"
#include "lith_world.h"

#define Themes(X) \
   X("Green")     \
   X("Rose")      \
   X("Umi")       \
   X("Ender")     \
   X("Orange")    \
   X("Grey")      \
   X("Basilissa") \
   X("Ghost")     \
   X("WinXP")

// Static Functions ----------------------------------------------------------|

static void Lith_CBITab_Arsenal(gui_state_t *g, struct player *p)
{
   extern void Lith_CBITab_Upgrades(gui_state_t *g, struct player *p);
   extern void Lith_CBITab_Shop    (gui_state_t *g, struct player *p);

   __str const tabnames[] = {L("LITH_TAB_UPGRADES"),
                             L("LITH_TAB_SHOP")};
   Lith_GUI_Tabs(g, &CBIState(g)->arsetab, tabnames, 13, 13, 1);

   switch(CBIState(g)->arsetab) {
   case cbi_tab_arsenal_upgrades: Lith_CBITab_Upgrades(g, p); break;
   case cbi_tab_arsenal_shop:     Lith_CBITab_Shop    (g, p); break;
   }
}

static void Lith_CBITab_Stat(gui_state_t *g, struct player *p)
{
   extern void Lith_CBITab_CBI   (gui_state_t *g, struct player *p);
   extern void Lith_CBITab_Status(gui_state_t *g, struct player *p);

   __str const tabnames[] = {L("LITH_TAB_ATTRIBUTES"),
                             L("LITH_TAB_CBI")};
   Lith_GUI_Tabs(g, &CBIState(g)->stattab, tabnames, 13, 13, 1);

   switch(CBIState(g)->stattab) {
   case cbi_tab_stat_attr: Lith_CBITab_Status(g, p); break;
   case cbi_tab_stat_cbi:  Lith_CBITab_CBI   (g, p); break;
   }
}

static void Lith_CBITab_Info(gui_state_t *g, struct player *p)
{
   extern void Lith_CBITab_BIP       (gui_state_t *g, struct player *p);
   extern void Lith_CBITab_Log       (gui_state_t *g, struct player *p);
   extern void Lith_CBITab_Statistics(gui_state_t *g, struct player *p);
   extern void Lith_CBITab_Notes     (gui_state_t *g, struct player *p);

   __str const tabnames[] = {L("LITH_TAB_BIP"),
                             L("LITH_TAB_LOG"),
                             L("LITH_TAB_STATISTICS"),
                             L("LITH_TAB_NOTES")};
   Lith_GUI_Tabs(g, &CBIState(g)->infotab, tabnames, 13, 13, 1);

   switch(CBIState(g)->infotab) {
   case cbi_tab_info_bip:        Lith_CBITab_BIP       (g, p); break;
   case cbi_tab_info_log:        Lith_CBITab_Log       (g, p); break;
   case cbi_tab_info_statistics: Lith_CBITab_Statistics(g, p); break;
   case cbi_tab_info_notes:      Lith_CBITab_Notes     (g, p); break;
   }
}

// Extern Functions ----------------------------------------------------------|

__str Lith_ThemeName(uint num)
{
   #define X(n) n,
   static __str const themes[cbi_theme_max] = {Themes(X)};
   #undef X

   return themes[num >= cbi_theme_max ? 0 : num];
}

script
void Lith_PlayerUpdateCBIGUI(struct player *p)
{
   gui_state_t *g = &p->cbi.guistate;

   p->cbi.theme = p->getCVarI("lith_gui_theme");

   if(p->cbi.theme != p->cbi.oldtheme)
   {
      #define X(n) ":UI_" n ":",
      static __str const names[] = {Themes(X)};
      #undef X

      if(p->cbi.theme >= cbi_theme_max) p->cbi.theme = 0;

      p->cbi.guistate.gfxprefix = names[p->cbi.oldtheme = p->cbi.theme];
   }

   Lith_GUI_Begin(g, 320, 240);

   if(!p->indialogue)
      Lith_GUI_UpdateState(g, p);

   PrintSpriteA(StrParam("%SBackground", g->gfxprefix), 0,1, 0,1, 0.7);

   if(Lith_GUI_Button(g, .x = 296, 13, Pre(btnexit)))
      p->useGUI(GUI_CBI);

   __str tabnames[] = {Language("LITH_TAB_ARSENAL_%S", p->discrim),
                       L("LITH_TAB_STATUS"),
                       L("LITH_TAB_INVENTORY"),
                       L("LITH_TAB_INFO"),
                       L("LITH_TAB_SETTINGS")};
   Lith_GUI_Tabs(g, &CBIState(g)->maintab, tabnames, 13, 13, 0);

   extern void Lith_CBITab_Items   (gui_state_t *g, struct player *p);
   extern void Lith_CBITab_Settings(gui_state_t *g, struct player *p);

   switch(CBIState(g)->maintab)
   {
   case cbi_tab_arsenal:  Lith_CBITab_Arsenal (g, p); break;
   case cbi_tab_status:   Lith_CBITab_Stat    (g, p); break;
   case cbi_tab_items:    Lith_CBITab_Items   (g, p); break;
   case cbi_tab_info:     Lith_CBITab_Info    (g, p); break;
   case cbi_tab_settings: Lith_CBITab_Settings(g, p); break;
   }

   Lith_GUI_End(g, p->getCVarI("lith_gui_cursor"));
}

void Lith_PlayerResetCBIGUI(struct player *p)
{
   p->cbi.guistate.cx = 320 / 2;
   p->cbi.guistate.cy = 240 / 2;

   Lith_GUI_Init(&p->cbi.guistate, &p->cbi.st);

   p->cbi.st.upgrselold = -1;

   p->cbi.oldtheme = -1;

   p->bip.curcategory = BIPC_MAIN;
   p->bip.curpage = null;
}

// Scripts -------------------------------------------------------------------|

script ext("ACS") type("net")
void Lith_KeyOpenCBI(void)
{
   if(ACS_Timer() < 10) return;

   withplayer(LocalPlayer)
      p->useGUI(GUI_CBI);
}

// EOF

