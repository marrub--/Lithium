#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

#include <math.h>


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

worldinfo_t world;
payoutinfo_t payout;


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_CanonTime
//
__str Lith_CanonTime(bool shorttime)
{
   int seconds = 53 + (world.ticks / 35);
   int minutes = 30 + (seconds     / 60);
   int hours   = 14 + (minutes     / 60);
   int days    = 25 + (hours       / 24); // pls
   
   if(shorttime)
      return StrParam("%0.2i:%0.2i %i/7/49",
         hours % 24, minutes % 60, days);
   else
      return StrParam("%0.2i:%0.2i:%0.2i %i/7/1649",
         hours % 24, minutes % 60, seconds % 60, days);
}

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
   fixed64_t taxpct = ACS_RandomFixed(1 / 100.0, 5 / 100.0);
   
   #define GenPay(name) \
      if(payout.name##max) \
      { \
         payout.name##pct = (payout.name##num / (fixed64_t)payout.name##max) * 100; \
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
      
      // Create a zombie.
      if(ACS_Spawn("ZombieMan", x, y, 0, tid = ACS_UniqueTID(), 0))
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
         // A_NoBlocking is called on a zombie. This is in case the monster
         // pack you're using (like DRLA Monsters) uses item drops for Score.
         ACS_Delay(16);
         
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
// Extern Functions
//

//
// Lith_MakeSerious
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_MakeSerious()
{
   ACS_SetActorProperty(0, APROP_RenderStyle, STYLE_None);
   ACS_SetActorPropertyString(0, APROP_ActiveSound, "silence");
   ACS_SetActorPropertyString(0, APROP_DeathSound,  "silence");
   ACS_SetActorPropertyString(0, APROP_PainSound,   "silence");
}

//
// Lith_GetPlayerData
//
[[__call("ScriptS"), __extern("ACS")]]
int Lith_GetPlayerData(int info, int permutation, bool target)
{
   if(target)
      ACS_SetActivatorToTarget(0);
   
   if(ACS_PlayerNumber() < 0)
      return 0;
   
   player_t *p = LocalPlayer;
   
   switch(info)
   {
   case pdata_upgrade:        return p->upgrades[permutation].active;
   case pdata_rifle_firemode: return p->riflefiremode;
   case pdata_buttons:        return p->buttons;
// case pdata_has_sigil:      return p->sigil.acquired;
   case pdata_weapon_zoom:    return bitsk(Lith_GetPCVarFixed(p, "lith_weapons_zoomfactor"));
   }
   
   return 0;
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
   
   if(world.mapnum == 1911777)
   {
      ACS_Exit_Normal(0);
      return;
   }
   
   static bool gsinit;
   static bool firstmap = true;
   
   // Init global/static state.
   {
      extern void Lith_GSInit_Upgrade(bool first);
      extern void Lith_GSInit_Shop(bool first);
      
      Lith_GSInit_Upgrade(!gsinit);
      Lith_GSInit_Shop(!gsinit);
   }
   
   if(!gsinit)
   {
      extern void Lith_GSInit_Weapon(void);
      
      Lith_GSInit_Weapon();
      
      Lith_CheckIfEnemiesAreCompatible();
      
      __with(int tid;)
         if((world.legendoom = ACS_SpawnForced("LDLegendaryMonsterMarker", 0, 0, 0, tid = ACS_UniqueTID(), 0)))
            ACS_Thing_Remove(tid);
      
      world.drlamonsters = ACS_GetCVar("DRLA_is_using_monsters");
      
      gsinit = true;
   }
   
   // Init a random seed from the map.
   world.mapseed = ACS_Random(0, 0x7FFFFFFF);
   
   // Init global score multiplier per-map.
   world.scoremul = round(ACS_GetCVarFixed("lith_sv_scoremul") * 100) / 100;
   
   // Seriously?
   if(ACS_GetCVar("lith_sv_seriousmode"))
   {
      world.scoremul += 15;
      ACS_SpawnForced("Lith_SeriousEmitter", 0, 0, 0);
   }
   
   // Give players some extra score if they're playing on extra hard or above.
   if(ACS_GameSkill() >= skill_extrahard)
      world.scoremul += 0.15;
   
   // Set the air control because ZDoom's default sucks.
   ACS_SetAirControl(0.77);
   
   // Sigil for when Lith_PlayerEntry can run.
   mapinit = true;
   
   // Delay so we can make sure players are initialized.
   ACS_Delay(1);
   
   // World-static init.
   if(ACS_Timer() <= 2)
   {
      // Payout, which is not done on the first map start.
      if(!firstmap)
         Lith_DoPayout();
      
      if(world.game == Game_Doom2 && world.cluster != world.prevcluster)
      {
         switch(world.prevcluster)
         {
         case 5: ForPlayer() p->bip.deliverMail("Cluster1"); break;
         case 6: ForPlayer() p->bip.deliverMail("Cluster2"); break;
         case 7: ForPlayer() p->bip.deliverMail("Cluster3"); break;
         }
      }
      
      world.prevcluster = world.cluster;
   }
   
   payout.killmax += world.mapkillmax;
   payout.itemmax += world.mapitemmax;
   
   firstmap = false;
   
   // Now we just check for things being gained so players get proper score.
   int prevsecrets = world.mapsecrets;
   int prevkills   = world.mapkills;
   int previtems   = world.mapitems;
   
   for(;;)
   {
      if(world.ticks > 17 * 35 * 60 * 60)
         ACS_Teleport_NewMap(1911777, 0, 0);
      
      int secrets = world.mapsecrets;
      int kills   = world.mapkills;
      int items   = world.mapitems;
      
      if(secrets > prevsecrets)
      {
         int delta = secrets - prevsecrets;
         Lith_GiveAllScore(9000 * delta, true);
         world.secretsfound += delta;
      }
      
      if(kills > prevkills) payout.killnum += kills - prevkills;
      if(items > previtems) payout.itemnum += items - previtems;
      
      prevsecrets = secrets;
      prevkills   = kills;
      previtems   = items;
      
      ACS_Delay(1);
      
      world.ticks++;
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
      p->closeGUI();
      Lith_PlayerDeltaStats(p);
   }
}

// EOF

