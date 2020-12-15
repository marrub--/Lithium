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

#define UData p->upgrdata.seven7

/* Extern Functions -------------------------------------------------------- */

void Upgr_7777777_Activate(struct player *p, struct upgrade *upgr) {
   SetGravity(0, 0.0);
}

void Upgr_7777777_Deactivate(struct player *p, struct upgrade *upgr) {
   SetGravity(0, 1.0);
}

script
void Upgr_7777777_Update(struct player *p, struct upgrade *upgr) {
   k32 velx, vely, velz = p->velz > 0 ? p->velz : -2;

   if(p->velz != 0) {
      if(!UData.in_air) {
         UData.in_air = true;
         SetFriction(0, 0.0);
         UData.fvel = p->getVel();
         UData.fyaw = p->yaw - ACS_VectorAngle(p->velx, p->vely);
      }
      k32 fyaw = UData.fyaw;
      fyaw = (i32)(fyaw * 100) / 100.0k;
      velx = ACS_Cos(p->yaw - fyaw) * UData.fvel;
      vely = ACS_Sin(p->yaw - fyaw) * UData.fvel;
   } else {
      UData.in_air = false;
      SetFriction(0, 1.0);
      velx = p->velx;
      vely = p->vely;
   }

   P_SetVel(p, velx, vely, velz);
}

/* EOF */
