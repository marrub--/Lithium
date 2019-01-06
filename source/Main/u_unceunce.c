// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// u_unceunce.c: UNCEUNCE upgrade.

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
