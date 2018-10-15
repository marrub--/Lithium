// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

#include <math.h>

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
void Lith_PunctuatorFire(void)
{
   withplayer(LocalPlayer)
   {
      int ptid = ACS_UniqueTID();

      ACS_LineAttack(0, p->yaw, p->pitch, 128, "Lith_PunctuatorPuff", "None", 2048.0, FHF_NORANDOMPUFFZ, ptid);

      if(ACS_ThingCount(T_NONE, ptid))
      {
         fixed x = ACS_GetActorX(ptid);
         fixed y = ACS_GetActorY(ptid);
         fixed z = ACS_GetActorZ(ptid);

         fixed64 yaw = atan2f(p->y - y, p->x - x);

         fixed64 ps = sinf(p->pitchf), cz = cosf(p->pitchf);
         fixed64 ys = sinf(yaw      ), yc = cosf(yaw      );

         fixed64 cx = ps * yc;
         fixed64 cy = ps * ys;

         for(int i = 0; i < 10; i++)
         {
            fixed64 sx = x + (cx * -(32 * i));
            fixed64 sy = y + (cy * -(32 * i));
            fixed64 sz = z + (cz * -(32 * i));

            int etid = ACS_UniqueTID();

            ACS_SpawnForced("Lith_PunctuatorExplosion", sx, sy, sz, etid);

            ACS_SetActivator(etid);
            ACS_SetPointer(AAPTR_TARGET, p->tid);
            p->setActivator();
         }
      }
   }
}

stkcall
void Upgr_PunctCannon_Deactivate(struct player *p, upgrade_t *upgr)
{
   InvGive("Lith_GTFO", 1);
}

// EOF

