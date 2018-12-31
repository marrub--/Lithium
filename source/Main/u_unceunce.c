// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "u_common.h"

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_UNCEUNCE_Activate(struct player *p, struct upgrade *upgr)
{
   ACS_SetMusic(sp_lmusic_Unce);
}

stkcall
void Upgr_UNCEUNCE_Deactivate(struct player *p, struct upgrade *upgr)
{
   ACS_SetMusic(sp_Any);
   ACS_FadeTo(0, 0, 0, 0, 0);
}

script
void Upgr_UNCEUNCE_Update(struct player *p, struct upgrade *upgr)
{
   k32 reeeed = ACS_Sin(p->ticks / 35.0) + 1.0 / 2.0;
   k32 greeen = ACS_Cos(p->ticks / 24.0) + 1.0 / 2.0;
   k32 bluuue = ACS_Sin(p->ticks / 13.0) + 1.0 / 2.0;

   ACS_FadeTo(reeeed * 255, greeen * 255, bluuue * 255, 0.6, TS);
   p->extrpitch += bluuue * 0.015;
   p->extryaw   += greeen * 0.01;
}

// EOF
