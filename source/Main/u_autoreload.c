// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_AutoReload_Activate(struct player *p, upgrade_t *upgr)
{
   p->autoreload = true;
}

stkcall
void Upgr_AutoReload_Deactivate(struct player *p, upgrade_t *upgr)
{
   p->autoreload = false;
}

// EOF

