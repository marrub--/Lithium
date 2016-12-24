#include "lith_common.h"
#include "lith_player.h"

// ---------------------------------------------------------------------------
// Callback scripts.
//

[[__call("ScriptS"), __script("Open")]]
static
void Lith_World(void)
{
   int maxsecrets = ACS_GetLevelInfo(LEVELINFO_TOTAL_SECRETS);
   int prevsecrets = 0;
   
   ACS_Delay(1);
   
   for(;;)
   {
      int secrets = ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS);
      
      if(secrets > prevsecrets)
         for(player_t *p = &players[0]; p < &players[MAX_PLAYERS]; p++)
            if(p->active)
            {
               Lith_GiveScore(p, 9000 * (secrets - prevsecrets));
               p->secretsfound++;
            }
      
      prevsecrets = secrets;
      
      // Eeeeeeeeeghhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
      for(player_t *p = &players[0]; p < &players[MAX_PLAYERS]; p++)
         if(p->active && ACS_CheckActorInventory(p->tid, "Lith_DivisionSigilSpriteTID"))
         {
            ACS_Thing_Remove(ACS_CheckActorInventory(p->tid, "Lith_DivisionSigilSpriteTID"));
            ACS_TakeActorInventory(p->tid, "Lith_DivisionSigilSpriteTID", 0x7FFFFFFF);
         }
      
      ACS_Delay(1);
   }
}

//
// ---------------------------------------------------------------------------

