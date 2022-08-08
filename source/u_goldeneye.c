// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Goldeneye upgrade.                                                       │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"

void Upgr_Goldeneye_Update(void) {
   if(ACS_Random(0, 0x7F) < 0x50) {
      k32 dist = ACS_RandomFixed(128, 256);
      k32 angl = ACS_RandomFixed(0, 1);
      ACS_SpawnForced(so_GEExplosion, pl.x + ACS_Cos(angl) * dist, pl.y + ACS_Sin(angl) * dist, pl.z + 32);
   }
}

/* EOF */
