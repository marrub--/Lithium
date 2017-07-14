#include "lith_upgrades_common.h"
#include "lith_world.h"

#define UData UData_HomingRPG(upgr)


//----------------------------------------------------------------------------
// Static Functions
//

//
// CheckTarget
//
[[__call("ScriptS")]]
static int CheckTarget(player_t *p)
{
   if(ACS_SetActivator(0, AAPTR_PLAYER_GETTARGET))
      return Lith_UniqueID(0);
   else
      return 0;
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_HomingMissile
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_HomingMissile()
{
   player_t *p = Lith_GetPlayer(0, AAPTR_TARGET);
   ACS_SetPointer(AAPTR_TRACER, p->tid, AAPTR_TRACER);
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_HomingRPG_Update(player_t *p, upgrade_t *upgr)
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
      
      if(ACS_SetActivator(0, AAPTR_TRACER) && ACS_GetActorProperty(0, APROP_Health) > 0)
         ACS_SpawnForced("Lith_TargetMarker",
            ACS_GetActorX(0),
            ACS_GetActorY(0),
            ACS_GetActorZ(0) + (ACS_GetActorPropertyFixed(0, APROP_Height) / 2k));
   }
}

// EOF

