// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "p_hudid.h"

#include <math.h>

/* Update view bobbing when you get damaged. */
script static
void P_Ren_DamageBob(void) {
   if(!InvNum(so_PowerStrength) && pl.health < pl.oldhealth) {
      k64 angle = (k64)ACS_RandomFixed(tau, -tau);
      k64 distance;

      if(pl.bobyaw + pl.bobpitch > 0.05) {
         angle = lerplk(angle, atan2f(pl.bobpitch, pl.bobyaw), 0.25lk);
      }

      distance  = mag2k(pl.bobyaw, pl.bobpitch);
      distance += (pl.oldhealth - pl.health) / (k64)pl.maxhealth;
      distance *= 0.2lk;

      k64 ys = sinf(angle), yc = cosf(angle);
      pl.bobpitch = yc * distance;
      pl.bobyaw   = ys * distance;
      pl.bobroll  = (k64)ACS_Random(-1, 1) * distance * (k64)ACS_RandomFixed(0.5, 0.7);
   }

   if(pl.bobpitch) pl.bobpitch = lerplk(pl.bobpitch, 0.0, 0.1);
   if(pl.bobyaw  ) pl.bobyaw   = lerplk(pl.bobyaw,   0.0, 0.1);
   if(pl.bobroll ) pl.bobroll  = lerplk(pl.bobroll,  0.0, 0.1);
}

/* Update additive view. */
void P_Ren_View(void) {
   if(Paused) return;

   P_Ren_DamageBob();

   k64 addp = 0, addy = 0, addr = 0;

   if(CVarGetI(sc_player_damagebob)) {
      k64 bobmul = CVarGetK(sc_player_damagebobmul);
      addp += pl.bobpitch * bobmul;
      addy += pl.bobyaw   * bobmul;
      addr += pl.bobroll  * bobmul;
   }

   k32 sidemul = CVarGetK(sc_player_viewtilt) * 0.2k;
   if(sidemul && pl.sidev) {
      pl.extrroll = lerplk(pl.extrroll, -pl.sidev * sidemul, 0.10lk);
   }

   if(pl.extrpitch) pl.extrpitch = lerplk(pl.extrpitch, 0.0lk, 0.1lk);
   if(pl.extryaw  ) pl.extryaw   = lerplk(pl.extryaw,   0.0lk, 0.1lk);
   if(pl.extrroll ) pl.extrroll  = lerplk(pl.extrroll,  0.0lk, 0.1lk);

   pl.addpitch = addp + pl.extrpitch;
   pl.addyaw   = addy + pl.extryaw;
   pl.addroll  = addr + pl.extrroll;
}

/* EOF */
