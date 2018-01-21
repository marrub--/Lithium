// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#ifndef LITH_CBI_H
#define LITH_CBI_H

#include "lith_gui.h"

#define CBIState(g) ((struct cbistate *)((g)->state))

// Type Definitions ----------------------------------------------------------|

enum
{
   cbi_tab_arsenal,
   cbi_tab_status,
   cbi_tab_items,
   cbi_tab_info,
   cbi_tab_settings,
   cbi_tab_max
};

enum
{
   cbi_tab_arsenal_upgrades,
   cbi_tab_arsenal_shop,
   cbi_tab_arsenal_max
};

enum
{
   cbi_tab_stat_attr,
   cbi_tab_stat_cbi,
   cbi_tab_stat_max
};

enum
{
   cbi_tab_info_bip,
   cbi_tab_info_log,
   cbi_tab_info_statistics,
   cbi_tab_info_max
};

enum
{
   cbi_theme_green,
   cbi_theme_rose,
   cbi_theme_umi,
   cbi_theme_ender,
   cbi_theme_orange,
   cbi_theme_grey,
   cbi_theme_bassilla,
   cbi_theme_ghost,
   cbi_theme_winxp,
   cbi_theme_max
};

typedef struct cbi_s
{
   gui_state_t guistate;
   int theme, oldtheme;

   struct cbistate
   {
      int maintab;
      int stattab;
      int arsetab;
      int infotab;

      gui_typeon_state_t biptypeon;
      gui_scroll_state_t bipscr;
      gui_scroll_state_t bipinfoscr;
      gui_txtbox_state_t bipsearch;

      void              *logsel;
      gui_scroll_state_t logscr;

      gui_scroll_state_t settingscr;

      int                shopsel;
      gui_scroll_state_t shopscr;

      int                upgrsel;
      int                upgrselold;
      int                upgrfilter;
      gui_typeon_state_t upgrtypeon;
      gui_scroll_state_t upgrscr;
   } st;

   int pruse;
} cbi_t;

// Extern Functions ----------------------------------------------------------|

void Lith_PlayerResetCBIGUI(struct player *p);

#endif

