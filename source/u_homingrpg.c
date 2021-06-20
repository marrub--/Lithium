/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * HomingRPG upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"
#include "w_world.h"

#define UData pl.upgrdata.homingrpg

/* Static Functions -------------------------------------------------------- */

script static
i32 CheckTarget()
{
   if(ACS_SetActivator(0, AAPTR_PLAYER_GETTARGET))
      return UniqueID(0);
   else
      return 0;
}

/* Extern Functions -------------------------------------------------------- */

script
void Upgr_HomingRPG_Update(struct upgrade *upgr)
{
   if(P_Wep_CurType() == weapon_launcher)
   {
      if(pl.buttons & BT_ALTATTACK)
      {
         i32 id;
         if((id = CheckTarget()) && id != UData.id)
         {
            ACS_LocalAmbientSound(ss_weapons_rocket_mark, 127);
            ACS_SetPointer(AAPTR_TRACER, 0, AAPTR_PLAYER_GETTARGET);
            UData.id = id;
         }
      }

      if(ACS_SetActivator(0, AAPTR_TRACER) && GetHealth(0) > 0)
         ACS_SpawnForced(so_TargetMarker,
                         GetX(0), GetY(0), GetZ(0) + GetHeight(0) / 2k);
   }
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "HomingMissile")
void Sc_HomingMissile(void)
{
   if(!P_None() && PtrPlayerNumber(0, AAPTR_TARGET) >= 0)
      ACS_SetPointer(AAPTR_TRACER, pl.tid, AAPTR_TRACER);
}

/* EOF */
