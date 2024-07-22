// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ StealthSys upgrade.                                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"

#define udata pl.upgrdata.stealthsys

Decl_Deactivate void Upgr_StealthSys_Deactivate(void) {
   pl.alpha = 1;
   InvTake(so_StealthSystem, 1);
}

Decl_Update void Upgr_StealthSys_Update(void) {
   k32 vel = fastabsk(pl.getVel()) / 10.0;
   pl.alpha = udata.mulvel = lerpk(udata.mulvel, vel, 0.02);

   i32 time = udata.mulvel * 20;
   if(!time || ACS_Timer() % time == 0) InvGive(so_StealthSystem, 1);
   else                                 InvTake(so_StealthSystem, 1);
}

/* EOF */
