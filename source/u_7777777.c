/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * 7777777 upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

#define UData pl.upgrdata.seven7

/* Extern Functions -------------------------------------------------------- */

void Upgr_7777777_Activate(struct upgrade *upgr) {
   SetGravity(0, 0.0);
}

void Upgr_7777777_Deactivate(struct upgrade *upgr) {
   SetGravity(0, 1.0);
}

script
void Upgr_7777777_Update(struct upgrade *upgr) {
   k32 velx, vely, velz = pl.velz > 0 ? pl.velz : -2;

   if(pl.velz != 0) {
      if(!UData.in_air) {
         UData.in_air = true;
         SetFriction(0, 0.0);
         UData.fvel = pl.getVel();
         UData.fyaw = pl.yaw - ACS_VectorAngle(pl.velx, pl.vely);
      }
      k32 fyaw = UData.fyaw;
      fyaw = floork(fyaw * 100.0k) / 100.0k;
      velx = ACS_Cos(pl.yaw - fyaw) * UData.fvel;
      vely = ACS_Sin(pl.yaw - fyaw) * UData.fvel;
   } else {
      UData.in_air = false;
      SetFriction(0, 1.0);
      velx = pl.velx;
      vely = pl.vely;
   }

   P_SetVel(velx, vely, velz);
}

/* EOF */
