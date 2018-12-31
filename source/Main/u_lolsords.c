// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "u_common.h"

#define UData UData_lolsords(upgr)

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_lolsords_Activate(struct player *p, struct upgrade *upgr)
{
   UData.origweapon = p->weaponclass;
   InvGive(so_Sword, 1);
}

stkcall
void Upgr_lolsords_Deactivate(struct player *p, struct upgrade *upgr)
{
   InvTake(so_Sword, 1);
   ACS_SetWeapon(UData.origweapon);
}

script
void Upgr_lolsords_Update(struct player *p, struct upgrade *upgr)
{
   ACS_SetWeapon(so_Sword);
}

// EOF
