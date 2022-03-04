// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef NDEBUG
#include "common.h"
#include "p_player.h"
#include "p_hudid.h"

void P_Ren_Debug() {
   if(!dbglevel(log_devh)) return;

   Dbg_Stat(
      "exp: lv.%u %u/%u/%u\n"
      "x: %k\ny: %k\nz: %k\n"
      "vx: %k\nvy: %k\nvz: %k\nvel: %k\n"
      "a.y: %k\na.p: %k\na.r: %k\n"
      "ap.y: %lk\nap.p: %lk\nap.r: %lk\n"
      "rage: %k\n"
      "score: %lli\n"
      "speedmul: %i\njumpboost: %i\n",
      pl.attr.level, pl.attr.expprev, pl.attr.exp, pl.attr.expnext,
      pl.x, pl.y, pl.z,
      pl.velx, pl.vely, pl.velz, pl.getVel(),
      pl.yaw * 360, pl.pitch * 360, pl.roll * 360,
      pl.addyaw * 360, pl.addpitch * 360, pl.addroll * 360,
      pl.rage,
      pl.score,
      pl.speedmul, pl.jumpboost
   );

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
