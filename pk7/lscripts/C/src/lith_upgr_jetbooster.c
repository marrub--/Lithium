#include "lith_upgrades_common.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Update
//
[[__call("ScriptS")]]
void Upgr_JetBooster_Update(player_t *p, upgrade_t *upgr)
{
   if(p->rocketcharge < rocketcharge_max)
      p->rocketcharge++;
   
   if(p->frozen) return;
   
   fixed grounddist = p->z - p->floorz;
   
   if(ButtonPressed(p, BT_SPEED) && grounddist > 16.0 && p->rocketcharge >= rocketcharge_max)
   {
      fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);
      
      ACS_PlaySound(0, "player/rocketboost");
      ACS_GiveInventory("Lith_RocketBooster", 1);
      Lith_SetPlayerVelocity(p,
                           p->velx + (cosk(angle) * 16.0),
                           p->vely + (sink(angle) * 16.0),
                           10.0, false, true);
      
      p->rocketcharge = 0;
      p->leaped = false;
   }
}

// EOF

