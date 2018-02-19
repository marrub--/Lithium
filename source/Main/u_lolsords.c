// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

#define UData UData_lolsords(upgr)

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
void Upgr_lolsords_Activate(struct player *p, upgrade_t *upgr)
{
   UData.origweapon = p->weaponclass;
   InvGive("Lith_Sword", 1);
}

//
// Deactivate
//
void Upgr_lolsords_Deactivate(struct player *p, upgrade_t *upgr)
{
   InvTake("Lith_Sword", 1);
   ACS_SetWeapon(UData.origweapon);
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_lolsords_Update(struct player *p, upgrade_t *upgr)
{
   ACS_SetWeapon("Lith_Sword");
}

// EOF

