// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Main CBI GUI entry point.                                                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "p_hudid.h"
#include "m_list.h"
#include "w_world.h"

noinit static gtab_t tn[5];

static
void P_CBI_TabArsenal(struct gui_state *g) {
   if(get_bit(pcl_outcasts, pl.pclass)) {
      faststrcpy_str(tn[0], sl_tab_upgrades);
      faststrcpy_str(tn[1], sl_tab_shop);
      G_Tabs(g, &pl.cbi.st.arsetab, tn, 2, 1);
   }
   switch(pl.cbi.st.arsetab) {
   case cbi_tab_arsenal_upgrades: P_CBI_TabUpgrades(g); break;
   case cbi_tab_arsenal_shop:     P_CBI_TabShop    (g); break;
   }
}

static
void P_CBI_TabStat(struct gui_state *g) {
   faststrcpy_str(tn[0], sl_tab_attributes);
   faststrcpy_str(tn[1], sl_tab_cbi);
   faststrcpy_str(tn[2], sl_tab_statistics);

   G_Tabs(g, &pl.cbi.st.stattab, tn, 3, 1);

   switch(pl.cbi.st.stattab) {
   case cbi_tab_stat_attr:       P_CBI_TabStatus    (g); break;
   case cbi_tab_stat_cbi:        P_CBI_TabCBI       (g); break;
   case cbi_tab_stat_statistics: P_CBI_TabStatistics(g); break;
   }
}

static
void P_CBI_TabInfo(struct gui_state *g) {
   faststrcpy_str(tn[0], sl_tab_bip);
   faststrcpy_str(tn[1], sl_tab_tuts);
   faststrcpy_str(tn[2], sl_tab_log);
   faststrcpy_str(tn[3], sl_tab_notes);

   G_Tabs(g, &pl.cbi.st.infotab, tn, 4, 1);

   switch(pl.cbi.st.infotab) {
   case cbi_tab_info_bip:   P_CBI_TabBIP  (g); break;
   case cbi_tab_info_tuts:  P_CBI_TabTuts (g); break;
   case cbi_tab_info_log:   P_CBI_TabLog  (g); break;
   case cbi_tab_info_notes: P_CBI_TabNotes(g); break;
   }
}

script void P_CBI_PTick(void) {
   if(pl.modal != _gui_cbi) return;

   struct gui_state *g = &pl.cbi.guistate;

   pl.cbi.st.buyfill.tic = CVarGetI(sc_gui_buyfiller);

   pl.cbi.theme = CVarGetI(sc_gui_theme);

   if(pl.cbi.theme != pl.cbi.oldtheme) {
      static
      cstr const names[] = {
         #define cbi_theme_x(x) ":UI_" #x ":",
         #include "p_cbi.h"
      };

      if(pl.cbi.theme >= cbi_theme_max) pl.cbi.theme = 0;

      g->gfxprefix = names[pl.cbi.oldtheme = pl.cbi.theme];
   }

   G_Begin(g, 320, 240);

   G_WinBeg(g, &pl.cbi.st.mainwin);

   if(G_Button(g, .x = 283, 0, Pre(btnexit))) P_GUI_Use();

   faststrcpy_str(tn[0], sl_tab_arsenal);
   faststrcpy_str(tn[1], sl_tab_status);
   faststrcpy_str(tn[2], sl_tab_inventory);
   faststrcpy_str(tn[3], sl_tab_info);
   faststrcpy_str(tn[4], sl_tab_settings);

   G_Tabs(g, &pl.cbi.st.maintab, tn, 5, 0);

   switch(pl.cbi.st.maintab) {
   case cbi_tab_arsenal:  P_CBI_TabArsenal (g); break;
   case cbi_tab_status:   P_CBI_TabStat    (g); break;
   case cbi_tab_items:    P_CBI_TabItems   (g); break;
   case cbi_tab_info:     P_CBI_TabInfo    (g); break;
   case cbi_tab_settings: P_CBI_TabSettings(g); break;
   }

   G_WinEnd(g, &pl.cbi.st.mainwin);

   G_End(g, CVarGetI(sc_gui_cursor));
}

void P_CBI_PMinit(void) {
   pl.cbi.guistate.cx = 320 / 2;
   pl.cbi.guistate.cy = 240 / 2;

   pl.cbi.guistate.gfxprefix = ":UI:";

   pl.cbi.st.upgrsel = UPGR_MAX;

   pl.cbi.oldtheme = -1;

   bip.curcategory = _bipc_main;
   bip.curpage = nil;
}

script_str ext("ACS") type("net") addr(OBJ "KeyOpenCBI")
void Z_OpenCBI(void) {
   if(ACS_Timer() < 10) return;
   P_GUI_Use();
}

/* EOF */
