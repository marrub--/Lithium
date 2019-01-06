// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// u_defensenuke.c: DefenseNuke upgrade.

#include "u_common.h"

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_DefenseNuke_Enter(struct player *p, struct upgrade *upgr)
{
   InvGive(so_Nuke, 1);
}

// EOF
