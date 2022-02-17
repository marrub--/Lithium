// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ lolsords upgrade.                                                        │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_common.h"

#define udata pl.upgrdata.lolsords

/* Extern Functions -------------------------------------------------------- */

void Upgr_lolsords_Activate(struct upgrade *upgr)
{
   udata.origweapon = pl.weaponclass;
   InvGive(so_Sword, 1);
}

void Upgr_lolsords_Deactivate(struct upgrade *upgr)
{
   InvTake(so_Sword, 1);
   ACS_SetWeapon(udata.origweapon);
}

script
void Upgr_lolsords_Update(struct upgrade *upgr)
{
   ACS_SetWeapon(so_Sword);
}

/* EOF */
