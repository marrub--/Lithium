// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

StrEntOFF

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_7777777_Activate(struct player *p, upgrade_t *upgr)
{
   SetPropK(0, APROP_Gravity, 0.0);
}

stkcall
void Upgr_7777777_Deactivate(struct player *p, upgrade_t *upgr)
{
   SetPropK(0, APROP_Gravity, 1.0);
}

script
void Upgr_7777777_Update(struct player *p, upgrade_t *upgr)
{
   fixed vel = -2;
   if(p->velz > 0) vel = p->velz;
   p->setVel(p->velx, p->vely, vel);
}

// EOF

