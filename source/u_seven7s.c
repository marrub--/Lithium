// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Seven7s upgrade.                                                         │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"

#define udata pl.upgrdata.seven7s

void Upgr_Seven7s_Activate(void) {
   SetGravity(0, 0.0);
   ACS_SetAirControl(0.0);
}

void Upgr_Seven7s_Deactivate(void) {
   SetGravity(0, 1.0);
   DefaultAirControl();
}

void Upgr_Seven7s_Update(void) {
   k32 velx, vely, velz = pl.velz > 0 ? pl.velz : -2;
   if(pl.velz != 0) {
      if(!udata.in_air) {
         udata.in_air = true;
         udata.fvel = pl.getVel();
         udata.fyaw = pl.yaw - atan2k(pl.vely, pl.velx);
      }
      k32 fyaw = udata.fyaw;
      fyaw = floork(fyaw * 100.0k) / 100.0k;
      velx = ACS_Cos(pl.yaw - fyaw) * udata.fvel;
      vely = ACS_Sin(pl.yaw - fyaw) * udata.fvel;
   } else {
      udata.in_air = false;
      velx = pl.velx;
      vely = pl.vely;
   }
   P_SetVel(velx, vely, velz);
}

/* EOF */
