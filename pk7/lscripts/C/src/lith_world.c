#include "lith_common.h"
#include "lith_player.h"

// ---------------------------------------------------------------------------
// Callback scripts.
//

[[__call("ScriptI"), __script("Open")]]
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
         for(int i = 0; i < MAX_PLAYERS; i++)
         {
            player_t *p = &players[i];
            if(p->active)
            {
               Lith_GiveScore(p, 9000 * (secrets - prevsecrets));
               p->secretsfound++;
            }
         }
      
      prevsecrets = secrets;
      
      ACS_Delay(1);
   }
}

//
// ---------------------------------------------------------------------------

