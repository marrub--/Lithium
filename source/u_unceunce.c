// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ UNCEUNCE upgrade.                                                        │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_common.h"

/* Extern Functions -------------------------------------------------------- */

void Upgr_UNCEUNCE_Activate(struct upgrade *upgr)
{
   ACS_SetMusic(sp_lmusic_Unce);
}

void Upgr_UNCEUNCE_Deactivate(struct upgrade *upgr)
{
   ACS_SetMusic(sp_star);
   ACS_FadeTo(0, 0, 0, 0, 0);
}

script
void Upgr_UNCEUNCE_Update(struct upgrade *upgr)
{
   k32 reeeed = ACS_Sin(pl.ticks / 35.0) + 1.0 / 2.0;
   k32 greeen = ACS_Cos(pl.ticks / 24.0) + 1.0 / 2.0;
   k32 bluuue = ACS_Sin(pl.ticks / 13.0) + 1.0 / 2.0;

   ACS_FadeTo(reeeed * 255, greeen * 255, bluuue * 255, 0.6, 0.029);
   pl.extrpitch += bluuue * 0.015;
   pl.extryaw   += greeen * 0.01;
}

/* EOF */
