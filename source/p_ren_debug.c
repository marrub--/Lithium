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

void P_Ren_Debug(void) {
   if(!dbglevel(log_devh)) return;

   Dbg_Stat(
      _l("exp: lv."),
      _p(pl.attr.level), _c(' '), _p(pl.attr.expprev), _c('/'),
      _p(pl.attr.exp),   _c('/'), _p(pl.attr.expnext), _c('\n'),
      _l("x: "),         _p(pl.x),              _c('\n'),
      _l("y: "),         _p(pl.y),              _c('\n'),
      _l("z: "),         _p(pl.z),              _c('\n'),
      _l("vx: "),        _p(pl.velx),           _c('\n'),
      _l("vy: "),        _p(pl.vely),           _c('\n'),
      _l("vz: "),        _p(pl.velz),           _c('\n'),
      _l("v: "),         _p(pl.getVel()),       _c('\n'),
      _l("y: "),         _p(pl.yaw      * 360), _c('\n'),
      _l("p: "),         _p(pl.pitch    * 360), _c('\n'),
      _l("r: "),         _p(pl.roll     * 360), _c('\n'),
      _l("ay: "),        _p(pl.addyaw   * 360), _c('\n'),
      _l("ap: "),        _p(pl.addpitch * 360), _c('\n'),
      _l("ar: "),        _p(pl.addroll  * 360), _c('\n'),
      _l("rage: "),      _p(pl.rage),           _c('\n'),
      _l("score: "),     _p(pl.score),          _c('\n'),
      _l("scoremul: "),  _p(pl.scoremul),       _c('\n'),
      _l("speedmul: "),  _p(pl.speedmul),       _c('\n'),
      _l("jumpboost: "), _p(pl.jumpboost),      _c('\n')
   );

   SetSize(320, 240);

   ACS_BeginPrint();
   for(i32 i = 0; i < dbgstatnum; i++) {
      ACS_PrintString(dbgstat[i]);
   }
   for(i32 i = 0; i < dbgnotenum; i++) {
      ACS_PrintString(dbgnote[i]);
   }
   PrintTextA(sf_smallfnt, CR_WHITE, 10,1, 20,1, 0.2);
}
#endif

/* EOF */
