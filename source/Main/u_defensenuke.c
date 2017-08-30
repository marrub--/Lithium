// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Extern Functions ----------------------------------------------------------|

//
// Enter
//
void Upgr_DefenseNuke_Enter(player_t *p, upgrade_t *upgr)
{
   ACS_GiveInventory("Lith_Nuke", 1);
}

// EOF

