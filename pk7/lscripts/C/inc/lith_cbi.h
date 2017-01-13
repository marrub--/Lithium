#ifndef LITH_CBI_H
#define LITH_CBI_H

#include "lith_cbi_gui.h"


//----------------------------------------------------------------------------
// Type Definitions
//

enum
{
   cbi_tab_upgrades,
   cbi_tab_shop,
   cbi_tab_bip,
   cbi_tab_statistics,
   cbi_tab_settings,
   cbi_tab_max
};

enum
{
   st_maintab,
   st_upgrsel,
   st_upgrscr,
   st_shopsel,
   st_shopscr,
   st_bipscr,
   st_max
};

typedef struct cbi_s
{
   gui_state_t guistate;
   bool open;
} cbi_t;


//----------------------------------------------------------------------------
// Extern Functions
//

[[__call("ScriptS"), __extern("ACS"), __script("Net")]]
void Lith_KeyOpenCBI(void);

[[__call("ScriptS")]]
void Lith_PlayerUpdateCBI(struct player_s *p);
void Lith_PlayerResetCBI(struct player_s *p);

#endif

