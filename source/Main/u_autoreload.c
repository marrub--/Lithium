// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
void Upgr_AutoReload_Activate(player_t *p, upgrade_t *upgr)
{
   p->autoreload = true;
}

//
// Deactivate
//
void Upgr_AutoReload_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->autoreload = false;
}

// EOF

