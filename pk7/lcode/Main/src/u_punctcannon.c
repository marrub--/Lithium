#include "lith_upgrades_common.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Deactivate
//
void Upgr_PunctCannon_Deactivate(player_t *p, upgrade_t *upgr)
{
   ACS_GiveInventory("Lith_GTFO", 1);
}

//
// Lith_PunctuatorFire
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_PunctuatorFire(void)
{
   player_t *p = LocalPlayer;
   int ptid = ACS_UniqueTID();
   
   ACS_LineAttack(0, p->yaw, p->pitch, 128, "Lith_PunctuatorPuff", "None", 2048.0, FHF_NORANDOMPUFFZ, ptid);
   
   if(ACS_ThingCount(T_NONE, ptid))
   {
      fixed x = ACS_GetActorX(ptid);
      fixed y = ACS_GetActorY(ptid);
      fixed z = ACS_GetActorZ(ptid);
      
      float yaw = atan2f(y - p->y, x - p->x);
      
      float cx = sin(p->pitchf) * cos(yaw);
      float cy = sin(p->pitchf) * sin(yaw);
      float cz = cos(p->pitchf);
      
      for(int i = 0; i < 10; i++)
      {
         float sx = x + (cx * -(32 * i));
         float sy = y + (cy * -(32 * i));
         float sz = z + (cz * -(32 * i));
         
         int etid = ACS_UniqueTID();
         
         ACS_SpawnForced("Lith_PunctuatorExplosion", sx, sy, sz, etid);
         
         ACS_SetActivator(etid);
         ACS_SetPointer(AAPTR_TARGET, p->tid);
         ACS_SetActivator(p->tid);
      }
   }
}

// EOF

