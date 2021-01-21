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
script static void P_Ren_DamageBob(struct player *p)
{
   if(!InvNum(so_PowerStrength) && p->health < p->oldhealth) {
      k64 angle = (k64)ACS_RandomFixed(tau, -tau);
      k64 distance;

      if(p->bobyaw + p->bobpitch > 0.05)
         angle = lerplk(angle, atan2f(p->bobpitch, p->bobyaw), 0.25lk);

      distance  = mag2k(p->bobyaw, p->bobpitch);
      distance += (p->oldhealth - p->health) / (k64)p->maxhealth;
      distance *= 0.2lk;

      k64 ys = sinf(angle), yc = cosf(angle);
      p->bobyaw   = ys * distance;
      p->bobpitch = yc * distance;
   }

   if(p->bobpitch) p->bobpitch = lerplk(p->bobpitch, 0.0, 0.1);
   if(p->bobyaw  ) p->bobyaw   = lerplk(p->bobyaw,   0.0, 0.1);
}

/* Update additive view. */
void P_Ren_View(struct player *p)
{
   if(Paused) return;

   P_Ren_DamageBob(p);

   k64 addp = 0, addy = 0;

   if(p->getCVarI(sc_player_damagebob)) {
      k64 bobmul = p->getCVarK(sc_player_damagebobmul);
      addp += p->bobpitch * bobmul;
      addy += p->bobyaw   * bobmul;
   }

   if(p->extrpitch) p->extrpitch = lerplk(p->extrpitch, 0.0lk, 0.1lk);
   if(p->extryaw  ) p->extryaw   = lerplk(p->extryaw,   0.0lk, 0.1lk);

   p->addpitch = addp + p->extrpitch;
   p->addyaw   = addy + p->extryaw;

   ifauto(k32, mul, p->getCVarK(sc_player_viewtilt) * 0.2) {
      /**/ if(p->sidev  ) p->addroll = lerplk(p->addroll, -p->sidev * mul, 0.10);
      else if(p->addroll) p->addroll = lerplk(p->addroll, 0,               0.14);
   }
}

/* EOF */
