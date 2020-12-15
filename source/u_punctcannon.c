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

void Upgr_PunctCannon_Deactivate(struct player *p, struct upgrade *upgr) {
   InvGive(so_GTFO, 1);
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "PunctuatorFire")
void Sc_PunctuatorFire(void) {
   with_player(LocalPlayer) {
      struct k32v3 v = trace_from(p->yaw, p->pitch, 2048, p->attackheight);
      ACS_SpawnForced(so_PunctuatorPuff, v.x, v.y, v.z);

      k64 yaw = atan2f(p->y - v.y, p->x - v.x);

      k64 ps = sinf(p->pitchf), cz = cosf(p->pitchf);
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
         ACS_SetPointer(AAPTR_TARGET, p->tid);
         p->setActivator();
      }
   }
}

/* EOF */
