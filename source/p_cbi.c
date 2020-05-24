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

static void P_CBI_TabArsenal(struct gui_state *g, struct player *p) {
   extern void P_CBI_TabUpgrades(struct gui_state *g, struct player *p);
   extern void P_CBI_TabShop    (struct gui_state *g, struct player *p);

   char tn[2][20];
   LanguageVC(tn[0], LANG "TAB_UPGRADES");
   LanguageVC(tn[1], LANG "TAB_SHOP");
   G_Tabs(g, &CBIState(g)->arsetab, tn, countof(tn), 13, 13, 1);

   switch(CBIState(g)->arsetab) {
   case cbi_tab_arsenal_upgrades: P_CBI_TabUpgrades(g, p); break;
   case cbi_tab_arsenal_shop:     P_CBI_TabShop    (g, p); break;
   }
}

static void P_CBI_TabStat(struct gui_state *g, struct player *p) {
   extern void P_CBI_TabCBI       (struct gui_state *g, struct player *p);
   extern void P_CBI_TabStatus    (struct gui_state *g, struct player *p);
   extern void P_CBI_TabStatistics(struct gui_state *g, struct player *p);

   char tn[3][20];
   LanguageVC(tn[0], LANG "TAB_ATTRIBUTES");
   LanguageVC(tn[1], LANG "TAB_CBI");
   LanguageVC(tn[2], LANG "TAB_STATISTICS");
   G_Tabs(g, &CBIState(g)->stattab, tn, countof(tn), 13, 13, 1);

   switch(CBIState(g)->stattab) {
   case cbi_tab_stat_attr:       P_CBI_TabStatus    (g, p); break;
   case cbi_tab_stat_cbi:        P_CBI_TabCBI       (g, p); break;
   case cbi_tab_stat_statistics: P_CBI_TabStatistics(g, p); break;
   }
}

static void P_CBI_TabInfo(struct gui_state *g, struct player *p) {
   extern void P_CBI_TabBIP       (struct gui_state *g, struct player *p);
   extern void P_CBI_TabLog       (struct gui_state *g, struct player *p);
   extern void P_CBI_TabNotes     (struct gui_state *g, struct player *p);

   char tn[3][20];
   LanguageVC(tn[0], LANG "TAB_BIP");
   LanguageVC(tn[1], LANG "TAB_LOG");
   LanguageVC(tn[2], LANG "TAB_NOTES");
   G_Tabs(g, &CBIState(g)->infotab, tn, countof(tn), 13, 13, 1);

   switch(CBIState(g)->infotab) {
   case cbi_tab_info_bip:   P_CBI_TabBIP  (g, p); break;
   case cbi_tab_info_log:   P_CBI_TabLog  (g, p); break;
   case cbi_tab_info_notes: P_CBI_TabNotes(g, p); break;
   }
}

/* Extern Functions -------------------------------------------------------- */

script void P_CBI_PTick(struct player *p) {
   extern void P_CBI_TabItems   (struct gui_state *g, struct player *p);
   extern void P_CBI_TabSettings(struct gui_state *g, struct player *p);

   struct gui_state *g = &p->cbi.guistate;

   p->cbi.theme = p->getCVarI(sc_gui_theme);

   if(p->cbi.theme != p->cbi.oldtheme) {
      #define X(n) ":UI_" n ":"
      static cstr const names[] = {
         X("Green"),
         X("Rose"),
         X("Umi"),
         X("Ender"),
         X("Orange"),
         X("Grey"),
         X("Basilissa"),
         X("Ghost"),
         X("WinXP"),
         X("Trans"),
      };
      #undef X

      if(p->cbi.theme >= cbi_theme_max) p->cbi.theme = 0;

      g->gfxprefix = names[p->cbi.oldtheme = p->cbi.theme];
   }

   G_Begin(g, 320, 240);

   if(!p->dlg.active) G_UpdateState(g, p);

   ACS_BeginPrint();
   PrintChrSt(g->gfxprefix);
   PrintChrSt("Background");
   PrintSpriteA(ACS_EndStrParam(), 0,1, 0,1, 0.7);

   if(G_Button(g, .x = 296, 13, Pre(btnexit))) P_GUI_Use(p);

   char tn[5][20];
   LanguageCV(tn[0], LANG "TAB_ARSENAL_%s", p->discrim);
   LanguageVC(tn[1], LANG "TAB_STATUS");
   LanguageVC(tn[2], LANG "TAB_INVENTORY");
   LanguageVC(tn[3], LANG "TAB_INFO");
   LanguageVC(tn[4], LANG "TAB_SETTINGS");
   G_Tabs(g, &CBIState(g)->maintab, tn, countof(tn), 13, 13, 0);

   switch(CBIState(g)->maintab) {
   case cbi_tab_arsenal:  P_CBI_TabArsenal (g, p); break;
   case cbi_tab_status:   P_CBI_TabStat    (g, p); break;
   case cbi_tab_items:    P_CBI_TabItems   (g, p); break;
   case cbi_tab_info:     P_CBI_TabInfo    (g, p); break;
   case cbi_tab_settings: P_CBI_TabSettings(g, p); break;
   }

   G_End(g, p->getCVarI(sc_gui_cursor));
}

void P_CBI_PMinit(struct player *p) {
   p->cbi.guistate.cx = 320 / 2;
   p->cbi.guistate.cy = 240 / 2;

   G_Init(&p->cbi.guistate, &p->cbi.st);

   p->cbi.st.upgrsel = UPGR_MAX;

   p->cbi.oldtheme = -1;

   p->bip.curcategory = BIPC_MAIN;
   p->bip.curpage = nil;
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") type("net") addr(OBJ "KeyOpenCBI")
void Sc_OpenCBI(void) {
   if(ACS_Timer() < 10) return;
   with_player(LocalPlayer) P_GUI_Use(p);
}

/* EOF */
