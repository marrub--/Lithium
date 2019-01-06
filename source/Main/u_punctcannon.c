// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// u_punctcannon.c: PunctCannon upgrade.

#include "u_common.h"

#include <math.h>

// Extern Functions ----------------------------------------------------------|

script_str ext("ACS")
void Lith_PunctuatorFire(void)
{
   withplayer(LocalPlayer)
   {
      i32 ptid = ACS_UniqueTID();

      ACS_LineAttack(0, p->yaw, p->pitch, 128, so_PunctuatorPuff, s_None, 2048.0, FHF_NORANDOMPUFFZ, ptid);

      if(ACS_ThingCount(T_NONE, ptid))
      {
         k32 x = GetX(ptid);
         k32 y = GetY(ptid);
         k32 z = GetZ(ptid);

         k64 yaw = atan2f(p->y - y, p->x - x);

         k64 ps = sinf(p->pitchf), cz = cosf(p->pitchf);
         k64 ys = sinf(yaw      ), yc = cosf(yaw      );

         k64 cx = ps * yc;
         k64 cy = ps * ys;

         for(i32 i = 0; i < 10; i++)
         {
            k64 sx = x + (cx * -(32 * i));
            k64 sy = y + (cy * -(32 * i));
            k64 sz = z + (cz * -(32 * i));

            i32 etid = ACS_UniqueTID();

            ACS_SpawnForced(so_PunctuatorExplosion, sx, sy, sz, etid);

            ACS_SetActivator(etid);
            ACS_SetPointer(AAPTR_TARGET, p->tid);
            p->setActivator();
         }
      }
   }
}

stkcall
void Upgr_PunctCannon_Deactivate(struct player *p, struct upgrade *upgr)
{
   InvGive(so_GTFO, 1);
}

// EOF
