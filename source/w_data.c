// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Data exposing functions.                                                 │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "w_world.h"

script ext("ACS") addr(lsc_pdata)
i32 Z_LPData(i32 info, i32 permutation) {
   switch(info) {
   case _pdt_weapon:       return P_Wep_CurType();
   case _pdt_upgrade:      return get_bit(pl.upgrades[permutation].flags, _ug_active);
   case _pdt_riflemode:    return pl.riflefiremode;
   case _pdt_hassigil:     return pl.sgacquired;
   case _pdt_pclass:       return pl.pclass;
   case _pdt_semifrozen:   return pl.semifrozen > 0;
   case _pdt_addp:         return k32_to_i32(pl.addpitch * 360.0);
   case _pdt_addy:         return k32_to_i32(pl.addyaw   * 360.0);
   case _pdt_addr:         return k32_to_i32(pl.addroll  * 360.0);
   case _pdt_recoilp:      return k32_to_i32(pl.extrpitch);
   case _pdt_attr:         return pl.attr.attrs[permutation];
   case _pdt_alpha:        return k32_to_i32(pl.alpha);
   case _pdt_oldhealth:    return pl.old.health;
   case _pdt_hudenabled:   return pl.hudenabled;
   case _pdt_flashbattery: return pl.upgrdata.flashlight.battery;
   case _pdt_bossspawned:  return wl.bossspawned;
   case _pdt_soulsfreed:   return ml.soulsfreed;
   case _pdt_ptid:         return pl.tid;
   case _pdt_mapf:         return get_bit(ml.flag, permutation);
   case _pdt_mapc:         return get_msk(ml.flag, _mapf_cat);
   case _pdt_mapr:         return get_msk(ml.flag, _mapf_rain);
   case _pdt_mapk:         return get_msk(ml.flag, _mapf_kind);
   }
   return 0;
}

#define w_setptr_x(t) \
   script ext("ACS") addr(lsc_setptr##t) \
      void Z_SetPtr_##t(intptr_t p, t s) { \
      *(t *)p = s; \
   }
#include "w_world.h"

/* EOF */
