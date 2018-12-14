// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Extern Functions ----------------------------------------------------------|

script
void Upgr_Goldeneye_Update(struct player *p, upgrade_t *upgr)
{
   if(ACS_Random(0, 0x7F) < 0x50)
   {
      fixed dist = ACS_RandomFixed(128, 256);
      fixed angl = ACS_RandomFixed(0, 1);
      ACS_SpawnForced(OBJ "GEExplosion", p->x + ACS_Cos(angl) * dist, p->y + ACS_Sin(angl) * dist, p->z + 32);
   }
}

// EOF

