// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_DefenseNuke_Enter(struct player *p, upgrade_t *upgr)
{
   InvGive(OBJ "Nuke", 1);
}

// EOF

