// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ RPG_B upgrade.                                                           │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"
#include "w_world.h"

#define udata pl.upgrdata.rpg_b

script static
i32 CheckTarget(void) {
   if(ACS_SetActivator(0, AAPTR_PLAYER_GETTARGET)) {
      return UniqueID(0);
   } else {
      return 0;
   }
}

Decl_Update void Upgr_RPG_B_Update(void) {
   if(P_Wep_CurType() == weapon_m_rocket) {
      if(P_ButtonHeld(BT_ALTATTACK)) {
         i32 id;
         if((id = CheckTarget()) && id != udata.id) {
            AmbientSound(ss_weapons_rocket_mark, 1.0);
            ACS_SetPointer(AAPTR_TRACER, 0, AAPTR_PLAYER_GETTARGET);
            udata.id = id;
         }
      }

      if(ACS_SetActivator(0, AAPTR_TRACER) && GetHealth(0) > 0) {
         ACS_SpawnForced(so_TargetMarker,
                         GetX(0), GetY(0), GetZ(0) + GetHeight(0) / 2k);
      }
   }
}

script_str ext("ACS") addr(OBJ "HomingMissile")
void Z_HomingMissile(void) {
   if(PtrPlayerNumber(0, AAPTR_TARGET) >= 0) {
      ACS_SetPointer(AAPTR_TRACER, pl.tid, AAPTR_TRACER);
   }
}

/* EOF */
