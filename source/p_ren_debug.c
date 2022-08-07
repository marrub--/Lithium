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
   SetSize(640, 480);
   str sky1 = EDataS(_edt_sky1), sky2 = EDataS(_edt_sky2);
   ACS_BeginPrint();
   _l("mission%: "); _p(ml.missionprc); _c('\n');
   _l("exp: lv.");
   _p(pl.attr.level); _c(' '); _p(pl.attr.expprev); _c('/');
   _p(pl.attr.exp);   _c('/'); _p(pl.attr.expnext); _c('\n');
   _l("pos: ");        _v(pl.x, pl.y, pl.z);                   _c('\n');
   _l("vel: ");        _v(pl.velx, pl.vely, pl.velz);          _c('\n');
   _l("v: ");          _p(pl.getVel());                        _c('\n');
   _l("ang: ");        _v(pl.yaw, pl.pitch, pl.roll);          _c('\n');
   _l("addang: ");     _v(pl.addyaw, pl.addpitch, pl.addroll); _c('\n');
   _l("rage: ");       _p(pl.rage);                            _c('\n');
   _l("score: ");      _p(pl.score); _c('*'); _p(pl.scoremul); _c('\n');
   _l("speedmul: ");   _p(pl.speedmul);                        _c('\n');
   _l("jumpboost: ");  _p(pl.jumpboost);                       _c('\n');
   _l("sky: ");        _p(sky1); _c(','); _p(sky2);            _c('\n');
   _l("*target: ");    _p(pl.upgrdata.flashlight.target);      _c('\n');
   _l("*intensity: "); _p(pl.upgrdata.flashlight.intensity);   _c('\n');
   _l("*speed: ");     _p(pl.upgrdata.flashlight.speed);       _c('\n');
   _l("*battery: ");   _p(pl.upgrdata.flashlight.battery);     _c('\n');
   if(get_bit(wl.cbiupgr, cupg_d_shield)) {
      _l("shield: ");     _p((i32)pl.shield);  _c('\n');
      _l("shieldmax: ");  _p(pl.shieldmax);    _c('\n');
      _l("regen: ");      _p(pl.regenwait);    _c('\n');
      _l("regenmax: ");   _p(pl.regenwaitmax); _c('\n');
   }
   _l("scorethreshold: "); _p(wl.scorethreshold); _c('\n');
   PrintTextA(sf_smallfnt, CR_WHITE, 0,1, 240,0, 0.2);
}
#endif

/* EOF */
