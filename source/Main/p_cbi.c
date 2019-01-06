// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_cbi.c: Main CBI GUI entry point.

#include "common.h"
#include "p_player.h"
#include "p_hudid.h"
#include "m_list.h"
#include "w_world.h"

#define TABCHARS 20

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

static void Lith_CBITab_Arsenal(struct gui_state *g, struct player *p)
{
   extern void Lith_CBITab_Upgrades(struct gui_state *g, struct player *p);
   extern void Lith_CBITab_Shop    (struct gui_state *g, struct player *p);

   char tn[2][TABCHARS];
   LanguageVC(tn[0], LANG "TAB_UPGRADES");
   LanguageVC(tn[1], LANG "TAB_SHOP");
   Lith_GUI_Tabs(g, &CBIState(g)->arsetab, tn, 13, 13, 1);

   switch(CBIState(g)->arsetab) {
   case cbi_tab_arsenal_upgrades: Lith_CBITab_Upgrades(g, p); break;
   case cbi_tab_arsenal_shop:     Lith_CBITab_Shop    (g, p); break;
   }
}

static void Lith_CBITab_Stat(struct gui_state *g, struct player *p)
{
   extern void Lith_CBITab_CBI   (struct gui_state *g, struct player *p);
   extern void Lith_CBITab_Status(struct gui_state *g, struct player *p);

   char tn[2][TABCHARS];
   LanguageVC(tn[0], LANG "TAB_ATTRIBUTES");
   LanguageVC(tn[1], LANG "TAB_CBI");
   Lith_GUI_Tabs(g, &CBIState(g)->stattab, tn, 13, 13, 1);

   switch(CBIState(g)->stattab) {
   case cbi_tab_stat_attr: Lith_CBITab_Status(g, p); break;
   case cbi_tab_stat_cbi:  Lith_CBITab_CBI   (g, p); break;
   }
}

static void Lith_CBITab_Info(struct gui_state *g, struct player *p)
{
   extern void Lith_CBITab_BIP       (struct gui_state *g, struct player *p);
   extern void Lith_CBITab_Log       (struct gui_state *g, struct player *p);
   extern void Lith_CBITab_Statistics(struct gui_state *g, struct player *p);
   extern void Lith_CBITab_Notes     (struct gui_state *g, struct player *p);

   char tn[4][TABCHARS];
   LanguageVC(tn[0], LANG "TAB_BIP");
   LanguageVC(tn[1], LANG "TAB_LOG");
   LanguageVC(tn[2], LANG "TAB_STATISTICS");
   LanguageVC(tn[3], LANG "TAB_NOTES");
   Lith_GUI_Tabs(g, &CBIState(g)->infotab, tn, 13, 13, 1);

   switch(CBIState(g)->infotab) {
   case cbi_tab_info_bip:        Lith_CBITab_BIP       (g, p); break;
   case cbi_tab_info_log:        Lith_CBITab_Log       (g, p); break;
   case cbi_tab_info_statistics: Lith_CBITab_Statistics(g, p); break;
   case cbi_tab_info_notes:      Lith_CBITab_Notes     (g, p); break;
   }
}

// Extern Functions ----------------------------------------------------------|

char const *Lith_ThemeName(u32 num)
{
   #define X(n) n,
   static char const *themes[cbi_theme_max] = {Themes(X)};
   #undef X

   return themes[num >= cbi_theme_max ? 0 : num];
}

script
void Lith_PlayerUpdateCBIGUI(struct player *p)
{
   extern void Lith_CBITab_Items   (struct gui_state *g, struct player *p);
   extern void Lith_CBITab_Settings(struct gui_state *g, struct player *p);

   struct gui_state *g = &p->cbi.guistate;

   p->cbi.theme = p->getCVarI(sc_gui_theme);

   if(p->cbi.theme != p->cbi.oldtheme)
   {
      #define X(n) ":UI_" n ":",
      static char const *names[] = {Themes(X)};
      #undef X

      if(p->cbi.theme >= cbi_theme_max) p->cbi.theme = 0;

      g->gfxprefix = names[p->cbi.oldtheme = p->cbi.theme];
   }

   Lith_GUI_Begin(g, 320, 240);

   if(!p->indialogue) Lith_GUI_UpdateState(g, p);

   ACS_BeginPrint();
   PrintChrSt(g->gfxprefix);
   ACS_PrintString(sp_Background);
   PrintSpriteA(ACS_EndStrParam(), 0,1, 0,1, 0.7);

   if(Lith_GUI_Button(g, .x = 296, 13, Pre(btnexit))) p->useGUI(gui_cbi);

   char tn[5][TABCHARS];
   LanguageCV(tn[0], LANG "TAB_ARSENAL_%s", p->discrim);
   LanguageVC(tn[1], LANG "TAB_STATUS");
   LanguageVC(tn[2], LANG "TAB_INVENTORY");
   LanguageVC(tn[3], LANG "TAB_INFO");
   LanguageVC(tn[4], LANG "TAB_SETTINGS");
   Lith_GUI_Tabs(g, &CBIState(g)->maintab, tn, 13, 13, 0);

   switch(CBIState(g)->maintab) {
   case cbi_tab_arsenal:  Lith_CBITab_Arsenal (g, p); break;
   case cbi_tab_status:   Lith_CBITab_Stat    (g, p); break;
   case cbi_tab_items:    Lith_CBITab_Items   (g, p); break;
   case cbi_tab_info:     Lith_CBITab_Info    (g, p); break;
   case cbi_tab_settings: Lith_CBITab_Settings(g, p); break;
   }

   Lith_GUI_End(g, p->getCVarI(sc_gui_cursor));
}

void Lith_PlayerResetCBIGUI(struct player *p)
{
   p->cbi.guistate.cx = 320 / 2;
   p->cbi.guistate.cy = 240 / 2;

   Lith_GUI_Init(&p->cbi.guistate, &p->cbi.st);

   p->cbi.st.upgrselold = -1;

   p->cbi.oldtheme = -1;

   p->bip.curcategory = BIPC_MAIN;
   p->bip.curpage = nil;
}

// Scripts -------------------------------------------------------------------|

script_str ext("ACS") type("net") addr("Lith_KeyOpenCBI")
void Sc_OpenCBI(void)
{
   if(ACS_Timer() < 10) return;

   withplayer(LocalPlayer)
      p->useGUI(gui_cbi);
}

// EOF
