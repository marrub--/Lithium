#include "lith_upgrades_common.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Activate
//
void Upgr_7777777_Activate(player_t *p, upgrade_t *upgr)
{
   ACS_SetActorPropertyFixed(0, APROP_Gravity, 0.0);
}

//
// Deactivate
//
void Upgr_7777777_Deactivate(player_t *p, upgrade_t *upgr)
{
   ACS_SetActorPropertyFixed(0, APROP_Gravity, 1.0);
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_7777777_Update(player_t *p, upgrade_t *upgr)
{
   fixed vel = -2;
   if(p->velz > 0) vel += p->velz;
   Lith_SetPlayerVelocity(p, p->velx, p->vely, vel, false, true);
}

// EOF

