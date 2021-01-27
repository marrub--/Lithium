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
i32 Sc_LPData(i32 info, i32 permutation)
{
   union ik32 u;

   with_player(LocalPlayer) switch(info) {
   case pdata_weapon:     return P_Wep_CurType(p);
   case pdata_upgrade:    return get_bit(p->upgrades[permutation].flags, _ug_active);
   case pdata_riflemode:  return p->riflefiremode;
   case pdata_hassigil:   return p->sgacquired;
   case pdata_weaponzoom: return u.k = p->getCVarK(sc_weapons_zoomfactor), u.i;
   case pdata_slot3ammo:  return p->getCVarI(sc_weapons_slot3ammo);
   case pdata_pclass:     return p->pclass;
   case pdata_semifrozen: return p->semifrozen > 0;
   case pdata_addp:       return u.k = p->addpitch,  u.i;
   case pdata_addy:       return u.k = p->addyaw,    u.i;
   case pdata_recoilp:    return u.k = p->extrpitch, u.i;
   case pdata_attr:       return p->attr.attrs[permutation];
   case pdata_alpha:      return u.k = p->alpha, u.i;
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
   case wdata_ptid:   for_player() return p->tid;
   case wdata_pclass: for_player() return p->pclass;
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
