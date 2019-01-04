// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#include "u_common.h"

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_7777777_Activate(struct player *p, struct upgrade *upgr)
{
   SetPropK(0, APROP_Gravity, 0.0);
}

stkcall
void Upgr_7777777_Deactivate(struct player *p, struct upgrade *upgr)
{
   SetPropK(0, APROP_Gravity, 1.0);
}

script
void Upgr_7777777_Update(struct player *p, struct upgrade *upgr)
{
   k32 vel = -2;
   if(p->velz > 0) vel = p->velz;
   p->setVel(p->velx, p->vely, vel);
}

// EOF
