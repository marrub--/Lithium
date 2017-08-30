// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
void Upgr_CyberLegs_Activate(player_t *p, upgrade_t *upgr)
{
   p->speedmul += 0.2;
   p->jumpboost += 0.5;
}

//
// Deactivate
//
void Upgr_CyberLegs_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->speedmul -= 0.2;
   p->jumpboost -= 0.5;
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_CyberLegs_Update(player_t *p, upgrade_t *upgr)
{
   fixed absvel = absk(p->old.velz) * 10.0k;

   if(p->velz == 0 && absvel > 160)
   {
      for(fixed i = absvel; i >= 100; i -= 100)
      {
         int tid;
         ACS_SpawnForced("Lith_ExplodoBoots", p->x, p->y, p->z, tid = ACS_UniqueTID());
         ACS_SetActivator(tid);
         ACS_SetPointer(AAPTR_TARGET, p->tid);
         ACS_SetActivator(p->tid);
      }
   }
}

// EOF

