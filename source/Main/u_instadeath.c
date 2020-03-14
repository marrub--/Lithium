/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * InstaDeath upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

/* Extern Functions -------------------------------------------------------- */

script
void Upgr_InstaDeath_Update(struct player *p, struct upgrade *upgr)
{
   if(p->health < p->oldhealth)
      ServCallI(sm_DieNow);
}

/* EOF */
