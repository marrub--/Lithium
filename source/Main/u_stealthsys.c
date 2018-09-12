// Copyright © 2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

#define UData UData_StealthSys(upgr)

// Extern Functions ----------------------------------------------------------|

//
// Deactivate
//
void Upgr_StealthSys_Deactivate(struct player *p, upgrade_t *upgr)
{
   p->alpha = 1;
   InvTake("Lith_StealthSystem", 1);
}

//
// Update
//
script
void Upgr_StealthSys_Update(struct player *p, upgrade_t *upgr)
{
   fixed vel = absk(p->getVel()) / 10.0;
   p->alpha = UData.mulvel = lerpk(UData.mulvel, vel, 0.02);

   int time = UData.mulvel * 20;
   if(!time || ACS_Timer() % time == 0)
      InvGive("Lith_StealthSystem", 1);
   else
      InvTake("Lith_StealthSystem", 1);
}

// EOF
