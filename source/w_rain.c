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

static
bool lmvar rain_chk;
static
k32 lmvar rain_px, rain_py;
static
i32 lmvar rain_dist;

dynam_aut script
void W_DoRain(void) {
   ACS_Delay(1);

   pl.setActivator();
   StartSound(ss_amb_wind, lch_weather1, CHANF_LOOP, 0.001, ATTN_NONE);
   StartSound(ss_amb_rain, lch_weather2, CHANF_LOOP, 0.001, ATTN_NONE);

   k32 skydist, curskydist = 1;
   for(;;) {
      if((rain_chk = !ACS_CheckActorCeilingTexture(0, sp_F_SKY1))) {
         rain_dist = 1024;
         rain_px = pl.x;
         rain_py = pl.y;
      } else {
         InvTake(so_SMGHeat, 1);
      }

      if(!ml.islithmap && MapNum >= 21) {
         ServCallV(sm_SpawnRain, so_BloodRainDrop);
      } else if(MapNum == 1888001) {
         ServCallV(sm_SpawnRain, so_AbyssRainDrop);
      } else {
         ServCallV(sm_SpawnRain, so_RainDrop);
      }

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

script ext("ACS") addr(lsc_raindropspawn)
void Z_RainDropSpawn(void) {
   if(rain_chk) {
      i32 dist = mag2i(GetX(0) - rain_px, GetY(0) - rain_py);
      if(dist < rain_dist) rain_dist = dist;
   }
}

/* EOF */
