// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
stkcall
void Upgr_AutoReload_Activate(struct player *p, upgrade_t *upgr)
{
   p->autoreload = true;
}

//
// Deactivate
//
stkcall
void Upgr_AutoReload_Deactivate(struct player *p, upgrade_t *upgr)
{
   p->autoreload = false;
}

// EOF

