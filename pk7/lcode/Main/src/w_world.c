#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"


//----------------------------------------------------------------------------
// Static Objects
//

static bool enemiesarecompatible;
static bool enemycheckfinished;


//----------------------------------------------------------------------------
// Extern Objects
//

__addrdef __mod_arr Lith_MapVariable;
__addrdef __hub_arr Lith_WorldVariable;

bool Lith_MapVariable mapinit;
int  Lith_MapVariable mapid;

int secretsfound;
payoutinfo_t payout;
int mapseed;


//----------------------------------------------------------------------------
// Extern Functions
//

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

//
// Lith_EmitScore
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_EmitScore(int amount)
{
   if(enemycheckfinished || enemiesarecompatible)
      Lith_GiveAllScore(amount, false);
   
   enemiesarecompatible = true;
}


//----------------------------------------------------------------------------
// Static Functions
//

//
// Lith_DoPayout
//
static void Lith_DoPayout(void)
{
   long fixed taxpct = ACS_RandomFixed(1 / 100.0, 5 / 100.0);
   
   #define GenPay(name) \
      if(payout.name##max) \
      { \
         payout.name##pct = (payout.name##num / (long fixed)payout.name##max) * 100; \
         payout.name##scr = payout.name##pct * 500; \
      }
   
   GenPay(kill)
   GenPay(item)
   
   #undef GenPay
   
   payout.total  = payout.killscr + payout.itemscr;
   payout.total -= payout.tax = payout.total * taxpct;
   
   ForPlayer()
   {
      [[__call("ScriptS")]]
      extern void Lith_PlayerPayout(player_t *p);
      
      Lith_PlayerPayout(p);
   }
   
   memset(&payout, 0, sizeof(payout));
}

//
// Lith_CheckPlayer1Sight
//
[[__call("ScriptS")]]
static bool Lith_CheckPlayer1Sight(int tid)
{
   ACS_SetActivator(0, AAPTR_PLAYER1);
   return ACS_CheckSight(0, tid, 0);
}

//
// Lith_CheckIfEnemiesAreCompatible
//
[[__call("ScriptS")]]
static void Lith_CheckIfEnemiesAreCompatible(void)
{
   // If we have nomonsters on we have no reason to detect this.
   if(ACS_GetCVar("sv_nomonsters"))
      return;
   
   // If the user's confirmed the monsters are OK, we don't need to check.
   if(ACS_GetCVar("lith_sv_monsters_ok"))
   {
      enemycheckfinished = true;
      return;
   }
   
   for(;;)
   {
      fixed x = ACS_RandomFixed(-32765, 32765);
      fixed y = ACS_RandomFixed(-32765, 32765);
      int tid;
      
      // Create an Imp.
      if(ACS_Spawn("DoomImp", x, y, 0, tid = ACS_UniqueTID(), 0))
      {
         // If it can see the player, then we need to relocate it.
         if(Lith_CheckPlayer1Sight(tid))
         {
            ACS_Thing_Remove(tid);
            continue;
         }
         
         // Wait for RandomSpawner inheriting actors.
         ACS_Delay(1);
         
         // Make the actor undetectable.
         ACS_SetActorProperty(tid, APROP_RenderStyle, STYLE_None);
         ACS_SetActorPropertyString(tid, APROP_ActiveSound, "silence");
         ACS_SetActorPropertyString(tid, APROP_AttackSound, "silence");
         ACS_SetActorPropertyString(tid, APROP_DeathSound,  "silence");
         ACS_SetActorPropertyString(tid, APROP_PainSound,   "silence");
         ACS_SetActorPropertyString(tid, APROP_SeeSound,    "silence");
         ACS_GiveActorInventory(tid, "Lith_EnemyChecker", 1);
         
         // This delay is very specific -- it's the amount of time before
         // A_NoBlocking is called on an imp. This is in case the monster pack
         // you're using (like DRLA Monsters) uses item drops for Score.
         ACS_Delay(24);
         
         // Get rid of the enemy.
         ACS_Thing_Remove(tid);
         
         // If the enemy emitted score, then we can get out of the script.
         enemycheckfinished = true;
         if(enemiesarecompatible)
            return;
         
         break;
      }
   }
   
   // Let's at least be nice to the player.
   ACS_SetPlayerProperty(1, true, PROP_TOTALLYFROZEN);
   ACS_SetPlayerProperty(1, true, PROP_NOTARGET);
   ACS_SetPlayerProperty(1, true, PROP_BUDDHA);
   
   ACS_FadeTo(0, 0, 0, 0.5, 1);
   ACS_AmbientSound("misc/chat", 127);
   
   for(;;)
   {
      PrintBold("\CjThe monster mod you are using is \Cgnot compatible\Cj "
         "with Lithium.\n\CjPlease unload it.\n\n\CjIf you are \Cnabsolutely "
         "certain\Cj that it will work, or if you don't care,\n\Cjopen the "
         "console and type \"\Cdlith_sv_monsters_ok true\Cj\".");
      
      ACS_Delay(35);
   }
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
   if(ACS_GameType() == GAME_TITLE_MAP)
      return;
   
   static bool gsinit;
   static bool firstmap = true;
   
   // Init global/static state. This is only done once per game.
   if(!gsinit)
   {
      [[__extern("ACS"), __call("LangACS")]]
      extern void Lith_GSInit_Balance(void);
      extern void Lith_GSInit_Upgrade(void);
      extern void Lith_GSInit_Shop(void);
      extern void Lith_GSInit_Weapon(void);
      
      Lith_GSInit_Balance();
      Lith_GSInit_Upgrade();
      Lith_GSInit_Shop();
      Lith_GSInit_Weapon();
      
      Lith_CheckIfEnemiesAreCompatible();
      
      gsinit = true;
   }
   
   // Init a random seed for the map.
   mapseed = ACS_Random(0, 0x7FFFFFFF);
   
   // Give players some extra score if they're playing on extra hard or above.
   if(ACS_GameSkill() >= skill_extrahard)
      ForPlayer()
         p->scoremul += 0.15;
   
   // Set the air control because ZDoom's default sucks.
   ACS_SetAirControl(0.77);
   
   // Sigil for when Lith_PlayerEntry can run.
   mapinit = true;
   
   // Delay so we can make sure players are initialized.
   ACS_Delay(1);
   
   // World-static init.
   if(ACS_Timer() <= 2)
   {
      // Payout is not done on the first map start.
      if(!firstmap)
         Lith_DoPayout();
   }
   
   payout.killmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_MONSTERS);
   payout.itemmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_ITEMS);
   
   firstmap = false;
   
   // Now we just check for things being gained so players get proper score.
   int prevsecrets = ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS);
   int prevkills   = ACS_GetLevelInfo(LEVELINFO_KILLED_MONSTERS);
   int previtems   = ACS_GetLevelInfo(LEVELINFO_FOUND_ITEMS);
   
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

