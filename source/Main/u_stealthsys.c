// Copyright © 2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

#define UData UData_StealthSys(upgr)

// Extern Functions ----------------------------------------------------------|

//
// Deactivate
//
void Upgr_StealthSys_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->alpha = 1;
   ACS_TakeInventory("Lith_StealthSystem", 1);
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_StealthSys_Update(player_t *p, upgrade_t *upgr)
{
   fixed vel = absk(mag2k(p->velx, p->vely)) / 10.0;
   p->alpha = UData.mulvel = lerpk(UData.mulvel, vel, 0.02);

   int time = UData.mulvel * 20;
   if(!time || ACS_Timer() % time == 0)
      ACS_GiveInventory("Lith_StealthSystem", 1);
   else
      ACS_TakeInventory("Lith_StealthSystem", 1);
}

// EOF
