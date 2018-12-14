// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_upgrades_common.h"
#include "lith_world.h"

#define UData UData_HomingRPG(upgr)

// Static Functions ----------------------------------------------------------|

script
static int CheckTarget(struct player *p)
{
   if(ACS_SetActivator(0, AAPTR_PLAYER_GETTARGET))
      return Lith_UniqueID(0);
   else
      return 0;
}

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
void Lith_HomingMissile()
{
   struct player *p = Lith_GetPlayer(0, AAPTR_TARGET);
   ACS_SetPointer(AAPTR_TRACER, p->tid, AAPTR_TRACER);
}

script
void Upgr_HomingRPG_Update(struct player *p, upgrade_t *upgr)
{
   if(p->weapontype == weapon_launcher)
   {
      if(p->buttons & BT_ALTATTACK)
      {
         int id;
         if((id = CheckTarget(p)) && id != UData.id)
         {
            ACS_LocalAmbientSound("weapons/rocket/mark", 127);
            ACS_SetPointer(AAPTR_TRACER, 0, AAPTR_PLAYER_GETTARGET);
            UData.id = id;
         }
      }

      if(ACS_SetActivator(0, AAPTR_TRACER) && GetPropI(0, APROP_Health) > 0)
         ACS_SpawnForced(OBJ "TargetMarker", GetX(0), GetY(0), GetZ(0) + (GetPropK(0, APROP_Height) / 2k));
   }
}

// EOF

