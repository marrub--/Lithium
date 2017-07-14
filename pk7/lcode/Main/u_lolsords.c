#include "lith_upgrades_common.h"

#define UData UData_lolsords(upgr)


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Activate
//
void Upgr_lolsords_Activate(player_t *p, upgrade_t *upgr)
{
   UData.origweapon = p->weaponclass;
   ACS_GiveInventory("Lith_Sword", 1);
}

//
// Deactivate
//
void Upgr_lolsords_Deactivate(player_t *p, upgrade_t *upgr)
{
   ACS_TakeInventory("Lith_Sword", 1);
   ACS_SetWeapon(UData.origweapon);
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_lolsords_Update(player_t *p, upgrade_t *upgr)
{
   ACS_SetWeapon("Lith_Sword");
}

// EOF

