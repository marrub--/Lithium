#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"


//----------------------------------------------------------------------------
// Extern Objects
//

__addrdef __mod_arr Lith_MapVariable;
__addrdef __hub_arr Lith_WorldVariable;

bool Lith_MapVariable mapinit;
int  Lith_MapVariable mapid;

int secretsfound;
payoutinfo_t payout;


//----------------------------------------------------------------------------
// Extern Functions
//

[[__extern("ACS"), __call("LangACS")]] void Lith_SetupBalance(void);
void Lith_SetupWeaponsTables(void);
[[__call("ScriptS")]] void Lith_PlayerPayout(player_t *p);

//
// Lith_UniqueID
//
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

//
// Lith_World
//
[[__call("ScriptS"), __script("Open")]]
static void Lith_World(void)
{
   static bool gamestart = true;
   static bool gsinit;
   
   int prevsecrets = ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS);
   int prevkills   = ACS_GetLevelInfo(LEVELINFO_KILLED_MONSTERS);
   int previtems   = ACS_GetLevelInfo(LEVELINFO_FOUND_ITEMS);
   
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
   
   // Delay so we can make sure players are initialized.
   ACS_Delay(1);
   
   // Payout is world-static.
   if(!gamestart && ACS_Timer() <= 2)
   {
      long fixed taxpct = ACS_RandomFixed(1 / 100.0, 5 / 100.0);
      
#define GenPay(name) \
      if(payout.name##max) \
      { \
         payout.name##pct = (payout.name##num / (long fixed)payout.name##max) * 100; \
         payout.name##scr = payout.name##pct * 500; \
      } \
      else \
         payout.killpct = payout.killscr = 0;
      
      GenPay(kill)
      GenPay(item)
      
#undef GenPay
      
      payout.total  = payout.killscr + payout.itemscr;
      payout.total -= payout.tax = payout.total * taxpct;
      
      ForPlayer()
         Lith_PlayerPayout(p);
      
      memset(&payout, 0, sizeof(payout));
   }
   
   payout.killmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_MONSTERS);
   payout.itemmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_ITEMS);
   
   gamestart = false;
   
   // Now we just check for things being gained so players get proper score.
   for(;;)
   {
      int secrets = ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS);
      int kills   = ACS_GetLevelInfo(LEVELINFO_KILLED_MONSTERS);
      int items   = ACS_GetLevelInfo(LEVELINFO_FOUND_ITEMS);
      
      if(secrets > prevsecrets)
      {
         int delta = secrets - prevsecrets;
         Lith_GiveAllScore(9000 * delta, true);
         secretsfound += delta;
      }
      
      if(kills > prevkills) payout.killnum += kills - prevkills;
      if(items > previtems) payout.itemnum += items - previtems;
      
      prevsecrets = secrets;
      prevkills   = kills;
      previtems   = items;
      
      ACS_Delay(1);
   }
}

//
// Lith_WorldUnload
//
[[__call("ScriptS"), __script("Unloading")]]
static void Lith_WorldUnload(void)
{
   ForPlayer()
   {
      ACS_SetActivator(p->tid);
      Lith_PlayerDeinitUpgrades(p);
   }
}

// EOF

