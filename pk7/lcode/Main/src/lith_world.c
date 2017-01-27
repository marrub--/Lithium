#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"


//----------------------------------------------------------------------------
// Extern Objects
//

__addrdef __mod_arr Lith_MapVariable;

bool Lith_MapVariable mapinit;
int  Lith_MapVariable mapid;


//----------------------------------------------------------------------------
// Extern Functions
//

[[__extern("ACS"), __call("LangACS")]] void Lith_SetupBalance(void);
void Lith_SetupWeaponsTables(void);

int Lith_UniqueID(int tid)
{
   int pn;
   
   // Players return a unique identifier of -playernumber
   if((pn = Lith_GetPlayerNumber(tid)) != -1)
      return -(pn + 1);
   
   // If we already have a unique identifier, just return that,
   int id = Lith_CheckActorInventory(tid, "Lith_UniqueID");
   
   // otherwise we have to give a new unique identifier.
   if(id == 0)
      Lith_GiveActorInventory(tid, "Lith_UniqueID", id = ++mapid);
   
   return id;
}


//----------------------------------------------------------------------------
// Scripts
//

[[__call("ScriptS"), __script("Open")]]
static void Lith_World(void)
{
   static bool gsinit;
   
   // Init global/static state. This is only done once and is necessary so
   // the balance module can run and so the weapon tables can be built.
   if(!gsinit)
   {
      Lith_SetupBalance();
      Lith_SetupWeaponsTables();
      
      gsinit = true;
   }
   
   // Give players some extra score if they're playing on extra hard or above.
   if(ACS_GameSkill() >= skill_extrahard)
      ForPlayer()
         p->scoremul += 0.15;
   
   // Set the air control because ZDoom's default sucks.
   ACS_SetAirControl(0.77);
   
   // Sigil for when players are ready to go, because ENTER scripts can, for
   // some god damn reason, run before OPEN scripts.
   mapinit = true;
   
   // Now we just check for secrets being gained, so we can give score to
   // players when they're found.
   for(int prevsecrets = 0;;)
   {
      int secrets = ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS);
      
      if(secrets > prevsecrets)
         ForPlayer()
         {
            Lith_GiveScore(p, 9000 * (secrets - prevsecrets));
            p->secretsfound++;
         }
      
      prevsecrets = secrets;
      
      ACS_Delay(1);
   }
}

// EOF

