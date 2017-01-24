#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"


//----------------------------------------------------------------------------
// Extern Objects
//

__addrdef __mod_arr Lith_MapVariable;
bool Lith_MapVariable mapinit;


//----------------------------------------------------------------------------
// Extern Functions
//

[[__extern("ACS"), __call("LangACS")]] void Lith_SetupBalance(void);
void Lith_SetupWeaponsTables(void);


//----------------------------------------------------------------------------
// Scripts
//

[[__call("ScriptS"), __script("Open")]]
static void Lith_World(void)
{
   static bool staticinit;
   
   int prevsecrets = 0;
   
   if(!staticinit)
   {
      Lith_SetupBalance();
      Lith_SetupWeaponsTables();
      
      staticinit = true;
   }
   ACS_SetAirControl(0.77);
   
   mapinit = true;
   
   for(;;)
   {
      int secrets = ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS);
      
      if(secrets > prevsecrets)
         Lith_ForPlayer()
         {
            Lith_GiveScore(p, 9000 * (secrets - prevsecrets));
            p->secretsfound++;
         }
      
      prevsecrets = secrets;
      
      ACS_Delay(1);
   }
}

// EOF

