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

#include "u_all.h"

#define udata pl.upgrdata.lolsords

void Upgr_lolsords_Activate(void) {
   udata.origweapon = pl.weaponclass;
   InvGive(so_Sword, 1);
}

void Upgr_lolsords_Deactivate(void) {
   InvTake(so_Sword, 1);
   ACS_SetWeapon(udata.origweapon);
}

void Upgr_lolsords_Update(void) {
   ACS_SetWeapon(so_Sword);
}

/* EOF */
