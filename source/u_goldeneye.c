/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Goldeneye upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

/* Extern Functions -------------------------------------------------------- */

script
void Upgr_Goldeneye_Update(struct player *p, struct upgrade *upgr)
{
   if(ACS_Random(0, 0x7F) < 0x50)
   {
      k32 dist = ACS_RandomFixed(128, 256);
      k32 angl = ACS_RandomFixed(0, 1);
      ACS_SpawnForced(so_GEExplosion, p->x + ACS_Cos(angl) * dist, p->y + ACS_Sin(angl) * dist, p->z + 32);
   }
}

/* EOF */
