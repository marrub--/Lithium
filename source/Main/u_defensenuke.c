// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "u_common.h"

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_DefenseNuke_Enter(struct player *p, struct upgrade *upgr)
{
   InvGive(so_Nuke, 1);
}

// EOF
