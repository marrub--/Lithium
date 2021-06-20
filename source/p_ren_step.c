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

script void P_Ren_Step() {
   static
   struct {str nam, snd; i32 nxt;} const stepsnd[] = {
      {s"FWATER1", s"player/stepw", 11},
      {s"FWATER2", s"player/stepw", 11},
      {s"FWATER3", s"player/stepw", 11},
      {s"FWATER4", s"player/stepw", 11},
      {s"BLOOD1",  s"player/stepw", 15},
      {s"BLOOD2",  s"player/stepw", 15},
      {s"BLOOD3",  s"player/stepw", 15},
      {s"NUKAGE1", s"player/steps", 15},
      {s"NUKAGE2", s"player/steps", 15},
      {s"NUKAGE3", s"player/steps", 15},
      {s"SLIME01", s"player/steps", 12},
      {s"SLIME02", s"player/steps", 12},
      {s"SLIME03", s"player/steps", 12},
      {s"SLIME04", s"player/steps", 12},
      {s"SLIME05", s"player/steps", 12},
      {s"SLIME06", s"player/steps", 12},
      {s"SLIME07", s"player/steps", 12},
      {s"SLIME08", s"player/steps", 12},
      {s"LAVA1",   s"player/stepl", 17},
      {s"LAVA2",   s"player/stepl", 17},
      {s"LAVA3",   s"player/stepl", 17},
      {s"LAVA4",   s"player/stepl", 17},
   };

   if(Paused) return;

   if(pl.nextstep) {pl.nextstep--; return;}

   k32 dstmul = fastabsk(pl.getVel()) / 24.0;
   k32 vol    = CVarGetK(sc_player_footstepvol) * min(dstmul, 1);

   str floor = ACS_GetActorFloorTexture(0);
   str snd   = pl.stepnoise;
   i32 next  = 10;

   if(vol && pl.onground) {
      for(i32 i = 0; i < countof(stepsnd); i++)
         if(floor == stepsnd[i].nam) {snd = stepsnd[i].snd; next = stepsnd[i].nxt; break;}

      ACS_SetActivator(pl.cameratid);
      StartSound(snd, lch_body, 0, vol);
      pl.nextstep = next;
   }
}

/* EOF */
