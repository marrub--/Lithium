/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Data exposing functions.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"

/* Scripts ----------------------------------------------------------------- */

script ext("ACS") addr(lsc_pdata)
i32 Sc_LPData(i32 info, i32 permutation) {
   if(P_None()) return 0;

   union ik32 u;
   switch(info) {
   case pdata_weapon:     return P_Wep_CurType();
   case pdata_upgrade:    return get_bit(pl.upgrades[permutation].flags, _ug_active);
   case pdata_riflemode:  return pl.riflefiremode;
   case pdata_hassigil:   return pl.sgacquired;
   case pdata_weaponzoom: return u.k = CVarGetK(sc_weapons_zoomfactor), u.i;
   case pdata_pclass:     return pl.pclass;
   case pdata_semifrozen: return pl.semifrozen > 0;
   case pdata_addp:       return u.k = pl.addpitch * 360.0, u.i;
   case pdata_addy:       return u.k = pl.addyaw   * 360.0, u.i;
   case pdata_recoilp:    return u.k = pl.extrpitch, u.i;
   case pdata_attr:       return pl.attr.attrs[permutation];
   case pdata_alpha:      return u.k = pl.alpha, u.i;
   case pdata_oldhealth:  return pl.oldhealth;
   case pdata_hudenabled: return pl.hudenabled;
   }
   return 0;
}

script ext("ACS") addr(lsc_wdata)
i32 Sc_LWData(i32 info)
{
   switch(info) {
   case wdata_bossspawned: return bossspawned;
   case wdata_soulsfreed:  return soulsfreed;
   case wdata_dorain:      return dorain;
   case wdata_ptid:        return pl.tid;
   case wdata_pclass:      return pl.pclass;
   }

   return 0;
}

#define w_setptr_x(t) \
   script ext("ACS") addr(lsc_setptr##t) \
      void Sc_SetPtr_##t(intptr_t p, t s) { \
      *(t *)p = s; \
   }
#include "w_world.h"

/* EOF */
