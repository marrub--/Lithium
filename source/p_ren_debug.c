/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef NDEBUG
#include "common.h"
#include "p_player.h"
#include "p_hudid.h"

void P_Ren_Debug(struct player *p) {
   if(!get_bit(dbglevel, log_devh)) return;

   Dbg_Stat("exp: lv.%u %u/%u/%u\n", p->attr.level, p->attr.expprev, p->attr.exp, p->attr.expnext);
   Dbg_Stat("x: %k\ny: %k\nz: %k\n", p->x, p->y, p->z);
   Dbg_Stat("vx: %k\nvy: %k\nvz: %k\nvel: %k\n", p->velx, p->vely, p->velz, p->getVel());
   Dbg_Stat("a.y: %k\na.p: %k\na.r: %k\n", p->yaw * 360, p->pitch * 360, p->roll * 360);
   Dbg_Stat("ap.y: %lk\nap.p: %lk\nap.r: %lk\n", p->addyaw * 360, p->addpitch * 360, p->addroll * 360);
   Dbg_Stat("rage: %k\n", p->rage);
   Dbg_Stat("score: %lli\n", p->score);

   SetSize(320, 240);

   ACS_BeginPrint();
   for(i32 i = 0; i < dbgstatnum; i++)
      ACS_PrintString(dbgstat[i]);
   for(i32 i = 0; i < dbgnotenum; i++)
      ACS_PrintString(dbgnote[i]);
   PrintTextA(sf_smallfnt, CR_WHITE, 10,1, 20,1, 0.2);
}
#endif

/* EOF */
