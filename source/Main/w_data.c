// Copyright © 2016-2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

extern bool dorain;

script ext("ACS")
int LPData(int info, int permutation, bool target)
{
   if(target) ACS_SetActivatorToTarget(0);

   union {fixed k; int i;} u;

   withplayer(LocalPlayer) switch(info)
   {
   case pdata_weapon:     return p->weapontype;
   case pdata_upgrade:    return p->getUpgrActive(permutation);
   case pdata_riflemode:  return p->riflefiremode;
   case pdata_hassigil:   return p->sgacquired;
   case pdata_weaponzoom: return u.k = p->getCVarK("lith_weapons_zoomfactor"), u.i;
   case pdata_recoil:     return u.k = p->getCVarK("lith_weapons_recoil"),     u.i;
   case pdata_slot3ammo:  return p->getCVarI("lith_weapons_slot3ammo");
   case pdata_pclass:     return p->pclass;
   case pdata_semifrozen: return p->semifrozen > 0;
   case pdata_addp:       return u.k = p->addpitch,  u.i;
   case pdata_addy:       return u.k = p->addyaw,    u.i;
   case pdata_recoilp:    return u.k = p->extrpitch, u.i;
   }

   return 0;
}

script ext("ACS")
int LWData(int info)
{
   switch(info)
   {
   case wdata_bossspawned: return world.bossspawned;
   case wdata_enemycheck:  return world.enemycheck;
   case wdata_fun:         return world.fun;
   case wdata_soulsfreed:  return world.soulsfreed;
   case wdata_dorain:      return dorain;
   case wdata_ptid:   Lith_ForPlayer() return p->tid;
   case wdata_pclass: Lith_ForPlayer() return p->pclass;
   }

   return 0;
}

// EOF
