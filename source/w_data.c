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
   if(P_None()) return 0;

   switch(info) {
   case pdata_weapon:       return P_Wep_CurType();
   case pdata_upgrade:      return get_bit(pl.upgrades[permutation].flags, _ug_active);
   case pdata_riflemode:    return pl.riflefiremode;
   case pdata_hassigil:     return pl.sgacquired;
   case pdata_weaponzoom:   return k32_to_i32(CVarGetK(sc_weapons_zoomfactor));
   case pdata_pclass:       return pl.pclass;
   case pdata_semifrozen:   return pl.semifrozen > 0;
   case pdata_addp:         return k32_to_i32(pl.addpitch * 360.0);
   case pdata_addy:         return k32_to_i32(pl.addyaw   * 360.0);
   case pdata_addr:         return k32_to_i32(pl.addroll  * 360.0);
   case pdata_recoilp:      return k32_to_i32(pl.extrpitch);
   case pdata_attr:         return pl.attr.attrs[permutation];
   case pdata_alpha:        return k32_to_i32(pl.alpha);
   case pdata_oldhealth:    return pl.oldhealth;
   case pdata_hudenabled:   return pl.hudenabled;
   case pdata_flashbattery: return pl.upgrdata.flashlight.battery;
   }
   return 0;
}

script ext("ACS") addr(lsc_wdata)
i32 Z_LWData(i32 info) {
   switch(info) {
   case wdata_bossspawned: return wl.bossspawned;
   case wdata_soulsfreed:  return wl.soulsfreed;
   case wdata_dorain:      return wl.dorain;
   case wdata_ptid:        return pl.tid;
   case wdata_pclass:      return pl.pclass;
   case wdata_skymap:      return CVarGetI(sc_sv_sky) && !ml.islithmap;
   case wdata_inhell:      return MapNum >= 21;
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
