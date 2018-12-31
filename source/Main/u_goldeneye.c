// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "u_common.h"

// Extern Functions ----------------------------------------------------------|

script
void Upgr_Goldeneye_Update(struct player *p, struct upgrade *upgr)
{
   if(ACS_Random(0, 0x7F) < 0x50)
   {
      k32 dist = ACS_RandomFixed(128, 256);
      k32 angl = ACS_RandomFixed(0, 1);
      ACS_SpawnForced(so_GEExplosion, p->x + ACS_Cos(angl) * dist, p->y + ACS_Sin(angl) * dist, p->z + 32);
   }
}

// EOF
