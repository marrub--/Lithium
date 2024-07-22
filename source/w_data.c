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

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

script ext("ACS") addr(lsc_pdata)
i32 Z_LPData(i32 info, i32 permutation) {
   static struct realtime rt;
   switch(info) {
   case _pdt_weapon:         return P_Wep_CurType();
   case _pdt_upgrade:        return get_bit(pl.upgrades[permutation].flags, _ug_active);
   case _pdt_riflemode:      return pl.riflefiremode;
   case _pdt_hassigil:       return pl.sgacquired;
   case _pdt_semifrozen:     return pl.semifrozen > 0;
   case _pdt_addp:           return k32_to_i32(pl.addpitch * 360.0);
   case _pdt_addy:           return k32_to_i32(pl.addyaw   * 360.0);
   case _pdt_addr:           return k32_to_i32(pl.addroll  * 360.0);
   case _pdt_recoilp:        return k32_to_i32(pl.extrpitch);
   case _pdt_attr:           return pl.attr.attrs[permutation];
   case _pdt_alpha:          return k32_to_i32(pl.alpha);
   case _pdt_oldhealth:      return pl.old.health;
   case _pdt_hudenabled:     return pl.hudenabled;
   case _pdt_lighton:        return pl.light.on;
   case _pdt_lightwason:     return pl.light.was_on;
   case _pdt_lightbattery:   return pl.light.battery;
   case _pdt_lightintensity: return k32_to_i32(pl.light.intensity);
   case _pdt_soulsfreed:     return ml.soulsfreed;
   case _pdt_ptid:           return pl.tid;
   case _pdt_mflg:           return get_bit(ml.flag, permutation);
   case _pdt_menv:           return get_msk(ml.flag, _mflg_env);
   case _pdt_rain:           return get_msk(ml.flag, _mflg_rain);
   case _pdt_mfunc:          return get_msk(ml.flag, _mflg_func);
   case _pdt_mphantom:       return get_msk(ml.flag, _mflg_boss);
   case _pdt_msky:           return get_msk(ml.flag, _mflg_sky);
   case _pdt_temperature:    return ml.temperature;
   case _pdt_humidity:       return ml.humidity;
   case _pdt_windspeed:      return ml.windspeed;
   case _pdt_hudlpos:        return pl.hudlpos;
   case _pdt_hudrpos:        return pl.hudrpos;
   case _pdt_hudhppos:       return pl.hudhppos;
   case _pdt_hudtop:         return pl.hudtop;
   case _pdt_hudtype:        return pl.hudtype;
   case _pdt_playercolor:    return P_PlayerColor(permutation);
   case _pdt_rt_hrs:
   case _pdt_rt_min: {
      i32 div = (i32)(wl.realtime % DAYS(1)) / 60;
      if(info == _pdt_rt_min) return div % 60;
      else                    return div / 60;
   }
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
