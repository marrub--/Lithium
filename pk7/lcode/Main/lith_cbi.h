#ifndef LITH_CBI_H
#define LITH_CBI_H

#include "lith_gui.h"


//----------------------------------------------------------------------------
// Type Definitions
//

enum
{
   cbi_tab_upgrades,
   cbi_tab_cbi,
   cbi_tab_shop,
   cbi_tab_bip,
   cbi_tab_settings,
   cbi_tab_log,
   cbi_tab_max
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
   cbi_theme_max
};

enum
{
   st_maintab,
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


//----------------------------------------------------------------------------
// Extern Functions
//

[[__call("ScriptS")]]
void Lith_PlayerUpdateCBIGUI(struct player *p);
void Lith_PlayerResetCBIGUI(struct player *p);

#endif

