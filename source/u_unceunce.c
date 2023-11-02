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

#include "u_all.h"

void Upgr_UNCEUNCE_Activate(void) {
   ACS_SetMusic(sp_lmusic_Unce);
}

void Upgr_UNCEUNCE_Deactivate(void) {
   ACS_SetMusic(sp_star);
   ACS_FadeTo(0, 0, 0, 0, 0);
}

void Upgr_UNCEUNCE_Update(void) {
   k32 reeeed = ACS_Sin(ACS_Timer() / 35.0) + 1.0 / 2.0;
   k32 greeen = ACS_Cos(ACS_Timer() / 24.0) + 1.0 / 2.0;
   k32 bluuue = ACS_Sin(ACS_Timer() / 13.0) + 1.0 / 2.0;

   ACS_FadeTo(reeeed * 255, greeen * 255, bluuue * 255, 0.6, 0.029);
   pl.extrpitch += bluuue * 0.015;
   pl.extryaw   += greeen * 0.01;
}

/* EOF */
