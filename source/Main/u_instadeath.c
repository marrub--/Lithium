// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "u_common.h"

// Extern Functions ----------------------------------------------------------|

script
void Upgr_InstaDeath_Update(struct player *p, struct upgrade *upgr)
{
   if(p->health < p->oldhealth)
      InvGive(so_Die, 1);
}

// EOF
