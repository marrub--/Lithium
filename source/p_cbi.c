/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Main CBI GUI entry point.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "p_hudid.h"
#include "m_list.h"
#include "w_world.h"

/* Static Functions -------------------------------------------------------- */

static void P_CBI_TabArsenal(struct gui_state *g) {
   char tn[2][20];
   LanguageVC(tn[0], LANG "TAB_UPGRADES");
   LanguageVC(tn[1], LANG "TAB_SHOP");
   G_Tabs(g, &CBIState(g)->arsetab, tn, countof(tn), 0, 0, 1);

   switch(CBIState(g)->arsetab) {
   case cbi_tab_arsenal_upgrades: P_CBI_TabUpgrades(g); break;
   case cbi_tab_arsenal_shop:     P_CBI_TabShop    (g); break;
   }
}

static void P_CBI_TabStat(struct gui_state *g) {
   char tn[3][20];
   LanguageVC(tn[0], LANG "TAB_ATTRIBUTES");
   LanguageVC(tn[1], LANG "TAB_CBI");
   LanguageVC(tn[2], LANG "TAB_STATISTICS");
   G_Tabs(g, &CBIState(g)->stattab, tn, countof(tn), 0, 0, 1);

   switch(CBIState(g)->stattab) {
   case cbi_tab_stat_attr:       P_CBI_TabStatus    (g); break;
   case cbi_tab_stat_cbi:        P_CBI_TabCBI       (g); break;
   case cbi_tab_stat_statistics: P_CBI_TabStatistics(g); break;
   }
}

static void P_CBI_TabInfo(struct gui_state *g) {
   char tn[3][20];
   LanguageVC(tn[0], LANG "TAB_BIP");
   LanguageVC(tn[1], LANG "TAB_LOG");
   LanguageVC(tn[2], LANG "TAB_NOTES");
   G_Tabs(g, &CBIState(g)->infotab, tn, countof(tn), 0, 0, 1);

   switch(CBIState(g)->infotab) {
   case cbi_tab_info_bip:   P_CBI_TabBIP  (g); break;
   case cbi_tab_info_log:   P_CBI_TabLog  (g); break;
   case cbi_tab_info_notes: P_CBI_TabNotes(g); break;
   }
}

/* Extern Functions -------------------------------------------------------- */

script void P_CBI_PTick() {
   struct gui_state *g = &pl.cbi.guistate;

   pl.cbi.theme = pl.getCVarI(sc_gui_theme);

   if(pl.cbi.theme != pl.cbi.oldtheme) {
      static cstr const names[] = {
         #define cbi_theme_x(x) ":UI_" #x ":",
         #include "p_cbi.h"
      };

      if(pl.cbi.theme >= cbi_theme_max) pl.cbi.theme = 0;

      g->gfxprefix = names[pl.cbi.oldtheme = pl.cbi.theme];
   }

   G_Begin(g, 320, 240);

   G_UpdateState(g);

   G_WinBeg(g, &CBIState(g)->mainwin);

   if(G_Button(g, .x = 283, 0, Pre(btnexit))) P_GUI_Use();

   char tn[5][20];
   LanguageCV(tn[0], LANG "TAB_ARSENAL_%s", pl.discrim);
   LanguageVC(tn[1], LANG "TAB_STATUS");
   LanguageVC(tn[2], LANG "TAB_INVENTORY");
   LanguageVC(tn[3], LANG "TAB_INFO");
   LanguageVC(tn[4], LANG "TAB_SETTINGS");
   G_Tabs(g, &CBIState(g)->maintab, tn, countof(tn), 0, 0, 0);

   switch(CBIState(g)->maintab) {
   case cbi_tab_arsenal:  P_CBI_TabArsenal (g); break;
   case cbi_tab_status:   P_CBI_TabStat    (g); break;
   case cbi_tab_items:    P_CBI_TabItems   (g); break;
   case cbi_tab_info:     P_CBI_TabInfo    (g); break;
   case cbi_tab_settings: P_CBI_TabSettings(g); break;
   }

   G_WinEnd(g, &CBIState(g)->mainwin);

   G_End(g, pl.getCVarI(sc_gui_cursor));
}

void P_CBI_PMinit(void) {
   pl.cbi.guistate.cx = 320 / 2;
   pl.cbi.guistate.cy = 240 / 2;

   pl.cbi.guistate.gfxprefix = ":UI:";
   pl.cbi.guistate.state     = &pl.cbi.st;

   pl.cbi.st.upgrsel = UPGR_MAX;

   pl.cbi.oldtheme = -1;

   pl.bip.curcategory = BIPC_MAIN;
   pl.bip.curpage = nil;
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") type("net") addr(OBJ "KeyOpenCBI")
void Sc_OpenCBI(void) {
   if(ACS_Timer() < 10) return;
   if(!P_None()) P_GUI_Use();
}

/* EOF */
