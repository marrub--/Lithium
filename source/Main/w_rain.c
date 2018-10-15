// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

static bool  lmvar rain_chk;
static fixed lmvar rain_px;
static fixed lmvar rain_py;
static int   lmvar rain_dist;

script
void Lith_DoRain()
{
   struct player *p = &players[0];
   p->setActivator();

   ACS_PlaySound(p->weathertid, "amb/wind", CHAN_BODY,  0.001, true, ATTN_NONE);
   ACS_PlaySound(p->weathertid, "amb/rain", CHAN_VOICE, 0.001, true, ATTN_NONE);

   fixed skydist, curskydist = 1;
   for(;;)
   {
      if((rain_chk = !ACS_CheckActorCeilingTexture(0, "F_SKY1")))
      {
         rain_dist = 1024;
         rain_px = p->x;
         rain_py = p->y;
      }
      else
         InvTake("Lith_SMGHeat", 1);

      if((InHell || InSecret) && !world.islithmap)
         HERMES("SpawnRain", "Lith_BloodRainDrop");
      else
         HERMES("SpawnRain", "Lith_RainDrop");

      ACS_Delay(1);

      if(rain_chk)
      {
         skydist = rain_dist / 1024.0;
         skydist = minmax(skydist, 0, 1);
      }
      else
         skydist = 0;

      curskydist = lerpk(curskydist, skydist, 0.035);
      ACS_SoundVolume(p->weathertid, CHAN_BODY,  1 - curskydist);
      ACS_SoundVolume(p->weathertid, CHAN_VOICE, 1 - curskydist);
   }
}

script ext("ACS")
void Lith_RainDropSpawn()
{
   if(rain_chk)
   {
      int dist =
         mag2i(ACS_GetActorX(0) - rain_px, ACS_GetActorY(0) - rain_py);
      if(dist < rain_dist)
         rain_dist = dist;
   }
}

// EOF
