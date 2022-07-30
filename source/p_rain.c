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

static bool lmvar rain_chk;
static k32  lmvar rain_px, rain_py;
static i32  lmvar rain_dist;

alloc_aut(0) script void P_DoRain(void) {
   str raindrop;
   switch(get_msk(ml.flag, _mflg_rain)) {
   default: return;
   case _rain_rain:  raindrop = so_RainDrop;      break;
   case _rain_blood: raindrop = so_BloodRainDrop; break;
   case _rain_abyss: raindrop = so_AbyssRainDrop; break;
   case _rain_snow:  raindrop = so_SnowDrop;      break;
   }
   StartSound(ss_amb_wind, lch_weather1, CHANF_LOOP, 0.001, ATTN_NONE);
   if(get_msk(ml.flag, _mflg_rain) != _rain_snow) {
      StartSound(ss_amb_rain, lch_weather2, CHANF_LOOP, 0.001, ATTN_NONE);
   }
   k32 skydist, curskydist = 1;
   for(;;) {
      if((rain_chk = !ACS_CheckActorCeilingTexture(0, sp_F_SKY1))) {
         rain_dist = 1024;
         rain_px = pl.x;
         rain_py = pl.y;
      } else {
         InvTake(so_SMGHeat, 1);
      }
      ServCallV(sm_SpawnRain, raindrop);
      ACS_Delay(1);
      if(rain_chk) {
         skydist = rain_dist / 1024.0;
         skydist = clampk(skydist, 0, 1);
      } else {
         skydist = 0;
      }
      curskydist = lerpk(curskydist, skydist, 0.035);
      ACS_SoundVolume(0, lch_weather1, 1 - curskydist);
      ACS_SoundVolume(0, lch_weather2, 1 - curskydist);
   }
}

alloc_aut(0) script ext("ACS") addr(lsc_raindropspawn) void Z_RainDropSpawn(i32 dist) {
   if(dist < rain_dist) {
      rain_dist = dist;
   }
}

/* EOF */
