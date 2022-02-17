// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ InstaDeath upgrade.                                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_common.h"

/* Extern Functions -------------------------------------------------------- */

script
void Upgr_InstaDeath_Update(struct upgrade *upgr)
{
   if(pl.health < pl.oldhealth)
      ServCallI(sm_DieNow);
}

/* EOF */
