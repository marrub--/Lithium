// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Extern Functions ----------------------------------------------------------|

//
// Update
//
[[__call("ScriptS")]]
void Upgr_InstaDeath_Update(struct player *p, upgrade_t *upgr)
{
   if(p->health < p->oldhealth)
      InvGive("Lith_Die", 1);
}

// EOF

