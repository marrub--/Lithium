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

   Dbg_Stat("exp: lv.%u %u/%u/%u\n", pl.attr.level, pl.attr.expprev, pl.attr.exp, pl.attr.expnext);
   Dbg_Stat("x: %k\ny: %k\nz: %k\n", pl.x, pl.y, pl.z);
   Dbg_Stat("vx: %k\nvy: %k\nvz: %k\nvel: %k\n", pl.velx, pl.vely, pl.velz, pl.getVel());
   Dbg_Stat("a.y: %k\na.p: %k\na.r: %k\n", pl.yaw * 360, pl.pitch * 360, pl.roll * 360);
   Dbg_Stat("ap.y: %lk\nap.p: %lk\nap.r: %lk\n", pl.addyaw * 360, pl.addpitch * 360, pl.addroll * 360);
   Dbg_Stat("rage: %k\n", pl.rage);
   Dbg_Stat("score: %lli\n", pl.score);

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
