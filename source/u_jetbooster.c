// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ JetBooster upgrade.                                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"

void Upgr_JetBooster_Update(void) {
   if(pl.frozen) return;

   if(P_ButtonPressed(BT_JUMP) && !pl.onground &&
      (!get_bit(pl.upgrades[UPGR_ReflexWetw].flags, _ug_active) ||
       pl.upgrdata.reflexwetw.leaped == 2) &&
      !has_status(so_StatJetBooster) &&
      !has_status(so_StatJetLag))
   {
      k32 angle = pl.yaw - atan2k(pl.sidev, pl.forwardv);

      add_status(so_StatJetBooster);
      StartSound(ss_player_lane_rocketboost, lch_auto, 0, 1.0k);
      P_SetVel(pl.velx + ACS_Cos(angle) * 16.0, pl.vely + ACS_Sin(angle) * 16.0, 10.0);
   }
}

/* EOF */
