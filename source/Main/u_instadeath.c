// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Extern Functions ----------------------------------------------------------|

script
void Upgr_InstaDeath_Update(struct player *p, upgrade_t *upgr)
{
   if(p->health < p->oldhealth)
      InvGive(OBJ "Die", 1);
}

// EOF

