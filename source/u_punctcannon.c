/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * PunctCannon upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

#include <math.h>

/* Extern Functions -------------------------------------------------------- */

void Upgr_PunctCannon_Deactivate(struct upgrade *upgr) {
   InvGive(so_GTFO, 1);
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "PunctuatorFire")
void Sc_PunctuatorFire(void) {
   if(!P_None()) {
      struct k32v3 v = trace_from(pl.yaw, pl.pitch, 2048, pl.attackheight);
      ACS_SpawnForced(so_PunctuatorPuff, v.x, v.y, v.z);

      k64 yaw = atan2f(pl.y - v.y, pl.x - v.x);

      k64 ps = sinf(pl.pitchf), cz = cosf(pl.pitchf);
      k64 ys = sinf(yaw      ), yc = cosf(yaw      );

      k64 cx = ps * yc;
      k64 cy = ps * ys;

      for(i32 i = 0; i < 10; i++) {
         k64 sx = v.x + cx * -(32 * i);
         k64 sy = v.y + cy * -(32 * i);
         k64 sz = v.z + cz * -(32 * i);

         i32 etid = ACS_UniqueTID();

         ACS_SpawnForced(so_PunctuatorExplosion, sx, sy, sz, etid);

         ACS_SetActivator(etid);
         ACS_SetPointer(AAPTR_TARGET, pl.tid);
         pl.setActivator();
      }
   }
}

/* EOF */
