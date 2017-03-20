#include "lith_upgrades_common.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Update
//
[[__call("ScriptS")]]
void Upgr_InstaDeath_Update(player_t *p, upgrade_t *upgr)
{
   if(p->health < p->old.health)
      ACS_GiveInventory("Lith_Die", 1);
}

// EOF

