#include "lith_upgrades_common.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Deactivate
//
void Upgr_PunctCannon_Deactivate(player_t *p, upgrade_t *upgr)
{
   ACS_GiveInventory("Lith_GTFO", 1);
}

// EOF

