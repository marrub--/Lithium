/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * CyberLegs upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

/* Extern Functions -------------------------------------------------------- */

void Upgr_CyberLegs_Activate(struct player *p, struct upgrade *upgr)
{
   p->speedmul += 0.2;
   p->jumpboost += 0.5;
}

void Upgr_CyberLegs_Deactivate(struct player *p, struct upgrade *upgr)
{
   p->speedmul -= 0.2;
   p->jumpboost -= 0.5;
}

script
void Upgr_CyberLegs_Update(struct player *p, struct upgrade *upgr)
{
   k32 absvel = fastabsk(p->old.velz) * 10.0k;

   if(p->velz == 0 && absvel > 160)
   {
      for(k32 i = absvel; i >= 100; i -= 100)
      {
         i32 tid;
         ACS_SpawnForced(so_ExplodoBoots, p->x, p->y, p->z, tid = ACS_UniqueTID());
         ACS_SetActivator(tid);
         ACS_SetPointer(AAPTR_TARGET, p->tid);
         p->setActivator();
      }
   }
}

/* EOF */
