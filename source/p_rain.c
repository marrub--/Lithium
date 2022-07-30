// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Weather code.                                                            │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "w_world.h"

/* TODO: convert into thinker */
alloc_aut(0) script void P_DoRain(void) {
   str raindrop;
   switch(get_msk(ml.flag, _mflg_rain)) {
   default: return;
   case _rain_rain:  raindrop = so_RainDrop;      break;
   case _rain_blood: raindrop = so_BloodRainDrop; break;
   case _rain_abyss: raindrop = so_AbyssRainDrop; break;
   case _rain_fire:  raindrop = so_Ash;           break;
   case _rain_snow:  raindrop = so_SnowDrop;      break;
   }
   StartSound(ss_amb_wind, lch_weather1, CHANF_LOOP, 0.001, ATTN_NONE);
   if(get_msk(ml.flag, _mflg_rain) != _rain_snow) {
      StartSound(ss_amb_rain, lch_weather2, CHANF_LOOP, 0.001, ATTN_NONE);
   }
   k32 tgtdist, visdist = 1;
   for(;;) {
      ACS_Delay(1);
      k32 curdist = ServCallK(sm_SpawnRain, raindrop);
      if(curdist == 0.0k) {
         tgtdist = 0.0k;
         InvTake(so_SMGHeat, 1);
      } else {
         tgtdist = clampk(curdist / 1024.0k, 0.0k, 1.0k);
      }
      visdist = lerpk(visdist, tgtdist, 0.035k);
      ACS_SoundVolume(0, lch_weather1, 1 - visdist);
      ACS_SoundVolume(0, lch_weather2, 1 - visdist);
   }
}

/* EOF */
