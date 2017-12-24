// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#ifndef LITH_CBI_H
#define LITH_CBI_H

#include "lith_gui.h"

// Type Definitions ----------------------------------------------------------|

enum
{
   cbi_tab_arsenal,
   cbi_tab_status,
   cbi_tab_items,
   cbi_tab_bip,
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

enum
{
   st_maintab,
   st_stattab,
   st_arsetab,
   st_upgrsel,
   st_upgrselold,
   st_upgrscr,
   st_upgrtypeon,
   st_upgrfilter,
   st_shopsel,
   st_shopscr,
   st_bipscr,
   st_biptypeon,
   st_bipinfoscr,
   st_bipsearch,
   st_settingscr,
   st_logscr,
   st_logsel,
   st_max
};

typedef struct cbi_s
{
   gui_state_t guistate;
   int theme, oldtheme;

   int pruse;
} cbi_t;

// Extern Functions ----------------------------------------------------------|

void Lith_PlayerResetCBIGUI(struct player *p);

#endif

