// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// p_cbi.h: CBI GUI data.

#ifndef p_cbi_h
#define p_cbi_h

#include "gui.h"

#define CBIState(g) ((struct cbistate *)((g)->state))

// Type Definitions ----------------------------------------------------------|

enum
{
   cbi_tab_arsenal,
   cbi_tab_status,
   cbi_tab_items,
   cbi_tab_info,
   cbi_tab_settings,
};

enum
{
   cbi_tab_arsenal_upgrades,
   cbi_tab_arsenal_shop,
};

enum
{
   cbi_tab_stat_attr,
   cbi_tab_stat_cbi,
};

enum
{
   cbi_tab_info_bip,
   cbi_tab_info_log,
   cbi_tab_info_statistics,
   cbi_tab_info_notes,
};

enum
{
   cbi_theme_green,
   cbi_theme_rose,
   cbi_theme_umi,
   cbi_theme_ender,
   cbi_theme_orange,
   cbi_theme_grey,
   cbi_theme_basilissa,
   cbi_theme_ghost,
   cbi_theme_winxp,
   cbi_theme_trans,
   cbi_theme_max
};

struct cbi
{
   struct gui_state guistate;

   struct cbistate
   {
      i32 maintab;
      i32 stattab;
      i32 arsetab;
      i32 infotab;

      struct gui_typ biptypeon;
      struct gui_scr bipscr;
      struct gui_scr bipinfoscr;
      struct gui_txt bipsearch;

      i32            logsel;
      struct gui_scr logscr;

      struct gui_scr settingscr;

      i32            shopsel;
      struct gui_scr shopscr;

      i32            upgrsel;
      i32            upgrselold;
      i32            upgrfilter;
      struct gui_scr upgrscr;

      bool           noteedit;
      struct gui_scr notescr;
      struct gui_txt notebox;
   } st;

   u32 theme;
   u32 oldtheme;
   u32 pruse;
};

// Extern Functions ----------------------------------------------------------|

void P_CBI_PMinit(struct player *p);
char const *ThemeName(u32 num);

#endif
