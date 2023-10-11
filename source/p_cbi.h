// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ CBI GUI data.                                                            │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#if defined(cbi_theme_x)

cbi_theme_x(Green)
cbi_theme_x(Rose)
cbi_theme_x(Umi)
cbi_theme_x(Ender)
cbi_theme_x(Orange)
cbi_theme_x(Grey)
cbi_theme_x(Basilissa)
cbi_theme_x(Ghost)
cbi_theme_x(WinXP)
cbi_theme_x(Trans)

#undef cbi_theme_x

#elif !defined(p_cbi_h)
#define p_cbi_h

#include "gui.h"

enum {
   cbi_tab_arsenal,
   cbi_tab_status,
   cbi_tab_items,
   cbi_tab_info,
   cbi_tab_settings,
};

enum {
   cbi_tab_arsenal_upgrades,
   cbi_tab_arsenal_shop,
};

enum {
   cbi_tab_stat_attr,
   cbi_tab_stat_cbi,
   cbi_tab_stat_statistics,
};

enum {
   cbi_tab_info_bip,
   cbi_tab_info_tuts,
   cbi_tab_info_log,
   cbi_tab_info_notes,
};

enum {
   #define cbi_theme_x(x) cbi_theme_##x,
   #include "p_cbi.h"
   cbi_theme_max
};

struct cbi {
   struct gui_state guistate;
   struct cbi_state {
      mem_size_t     maintab;
      mem_size_t     stattab;
      mem_size_t     arsetab;
      mem_size_t     infotab;
      mem_size_t     settingstab;
      struct gui_win mainwin;
      struct gui_scr bipscr;
      struct gui_scr bipinfoscr;
      struct gui_txt bipsearch;
      struct gui_scr tutscr;
      struct gui_scr settingscr;
      i32            shopsel;
      struct gui_scr shopscr;
      i32            upgrsel;
      i32            upgrfilter;
      struct gui_scr upgrscr;
      struct gui_scr notescr;
      struct gui_txt notebox;
      struct gui_fil buyfill;
   } st;
   i32 theme;
   i32 oldtheme;
   i32 pruse;
};

void CBI_InstallSpawned(void);

void P_CBI_TabUpgrades(struct gui_state *g);
void P_CBI_TabShop    (struct gui_state *g);

void P_CBI_TabCBI       (struct gui_state *g);
void P_CBI_TabStatus    (struct gui_state *g);
void P_CBI_TabStatistics(struct gui_state *g);

void P_CBI_TabBIP       (struct gui_state *g);
void P_CBI_TabTuts      (struct gui_state *g);
void P_CBI_TabLog       (struct gui_state *g);
void P_CBI_TabNotes     (struct gui_state *g);

void P_CBI_TabItems   (struct gui_state *g);
void P_CBI_TabSettings(struct gui_state *g);

void P_CBI_PMinit(void);

#endif
