// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ DefenseNuke upgrade.                                                     │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_common.h"

/* Extern Functions -------------------------------------------------------- */

void Upgr_DefenseNuke_Enter(struct upgrade *upgr)
{
   ServCallI(sm_Nuke);
}

/* EOF */
