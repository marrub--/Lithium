/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "p_hudid.h"

#include <math.h>

/* Update view bobbing when you get damaged. */
script static void P_Ren_DamageBob()
{
   if(!InvNum(so_PowerStrength) && pl.health < pl.oldhealth) {
      k64 angle = (k64)ACS_RandomFixed(tau, -tau);
      k64 distance;

      if(pl.bobyaw + pl.bobpitch > 0.05)
         angle = lerplk(angle, atan2f(pl.bobpitch, pl.bobyaw), 0.25lk);

      distance  = mag2k(pl.bobyaw, pl.bobpitch);
      distance += (pl.oldhealth - pl.health) / (k64)pl.maxhealth;
      distance *= 0.2lk;

      k64 ys = sinf(angle), yc = cosf(angle);
      pl.bobyaw   = ys * distance;
      pl.bobpitch = yc * distance;
   }

   if(pl.bobpitch) pl.bobpitch = lerplk(pl.bobpitch, 0.0, 0.1);
   if(pl.bobyaw  ) pl.bobyaw   = lerplk(pl.bobyaw,   0.0, 0.1);
}

/* Update additive view. */
void P_Ren_View()
{
   if(Paused) return;

   P_Ren_DamageBob();

   k64 addp = 0, addy = 0;

   if(CVarGetI(sc_player_damagebob)) {
      k64 bobmul = CVarGetK(sc_player_damagebobmul);
      addp += pl.bobpitch * bobmul;
      addy += pl.bobyaw   * bobmul;
   }

   if(pl.extrpitch) pl.extrpitch = lerplk(pl.extrpitch, 0.0lk, 0.1lk);
   if(pl.extryaw  ) pl.extryaw   = lerplk(pl.extryaw,   0.0lk, 0.1lk);

   pl.addpitch = addp + pl.extrpitch;
   pl.addyaw   = addy + pl.extryaw;

   ifauto(k32, mul, CVarGetK(sc_player_viewtilt) * 0.2) {
      /**/ if(pl.sidev  ) pl.addroll = lerplk(pl.addroll, -pl.sidev * mul, 0.10);
      else if(pl.addroll) pl.addroll = lerplk(pl.addroll, 0,               0.14);
   }
}

/* EOF */
