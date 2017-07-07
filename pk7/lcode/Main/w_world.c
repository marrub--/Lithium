#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

#include <math.h>


//----------------------------------------------------------------------------
// Static Objects
//

static bool enemiesarecompatible;
static bool enemycheckfinished;
static bool gsinit;

static int lmvar upgradesspawned[8];
static int lmvar upgradesspawnediter;


//----------------------------------------------------------------------------
// Extern Objects
//

__addrdef __mod_arr lmvar;
__addrdef __hub_arr lwvar;

bool lmvar mapinit;
int  lmvar mapid;

worldinfo_t world;
payoutinfo_t payout;


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_CanonTime
//
__str Lith_CanonTime(int type)
{
   int seconds = 53 + (world.ticks / 35);
   int minutes = 30 + (seconds     / 60);
   int hours   = 14 + (minutes     / 60);
   int days    = 25 + (hours       / 24); // pls
   
   switch(type)
   {
   case CANONTIME_FULL:
      return StrParam("%0.2i:%0.2i:%0.2i %i-7-1649",
         hours % 24, minutes % 60, seconds % 60, days);
   case CANONTIME_SHORT:
      return StrParam("%0.2i:%0.2i %i-7-49",
         hours % 24, minutes % 60, days);
   case CANONTIME_DATE:
      return StrParam("%i-7-1649", days);
   }
}

//
// Lith_UniqueID
//
int Lith_UniqueID(int tid)
{
   int pn;
   
   // Negative values are for players.
   if((pn = Lith_GetPlayerNumber(tid)) != -1)
      return -(pn + 1);
   
   // If we already have a unique identifier, return that.
   int id = Lith_CheckActorInventory(tid, "Lith_UniqueID");
   
   // Otherwise we have to give a new unique identifier.
   // Monsters occupy the first area of ID allocation.
   if(id == 0)
      Lith_GiveActorInventory(tid, "Lith_UniqueID", id = /*DMON_MAX +*/ ++mapid);
   
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
   
   player_t *p = Lith_LocalPlayer;
   
   switch(info)
   {
   case pdata_upgrade:
      __with(upgrade_t *upgr = p->getUpgr(permutation);)
         return upgr ? upgr->active : false;
   case pdata_rifle_firemode: return p->riflefiremode;
   case pdata_buttons:        return p->buttons;
   case pdata_has_sigil:      return p->sigil.acquired;
   case pdata_weapon_zoom:    return bitsk(Lith_GetPCVarFixed(p, "lith_weapons_zoomfactor"));
   }
   
   return 0;
}

//
// Lith_GetWorldData
//
[[__call("ScriptS"), __extern("ACS")]]
int Lith_GetWorldData(int info)
{
   switch(info)
   {
   case wdata_brightweps:  return ACS_GetUserCVar(0, "lith_player_brightweps");
   case wdata_gsinit:      return gsinit;
   case wdata_bossspawned: return world.bossspawned;
   }
   
   return 0;
}

//
// Lith_PickupCBIItem
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_PickupCBIItem(int num)
{
   static void Lith_InstallCBIItem(int num);
   
   player_t *p = Lith_LocalPlayer;
   
   switch(num)
   {
   #define Case(n, msg) case n: p->log(msg); break
   Case(1, "> Installed KSKK Spec. High-Grade CPU");
   Case(2, "> Installed KSKK Spec. Super High-Grade CPU");
   Case(3, "> Installed Armor Interface");
   Case(4, "> Installed Weapon Modification Device");
   Case(5, "> Installed Weapon Refactoring Device");
   Case(6, "> Installed Reality Distortion Interface");
   #undef Case
   }
   
   Lith_InstallCBIItem(num);
}

//
// Lith_CBIItemWasSpawned
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_CBIItemWasSpawned(int num)
{
   upgradesspawned[upgradesspawnediter++] = num;
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
   
   Lith_ForPlayer()
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
      if(ACS_SpawnForced("ZombieMan", x, y, 0, tid = ACS_UniqueTID(), 0))
      {
         // If it can see the player, then we need to relocate it.
         if(Lith_CheckPlayer1Sight(tid))
         {
            ACS_Thing_Remove(tid);
            continue;
         }
         
         // Wait for spawners.
         ACS_Delay(2);
         
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
         ACS_Delay(17);
         
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

//
// Lith_InstallCBIItem
//
static void Lith_InstallCBIItem(int num)
{
   switch(num)
   {
   #define Case(n, name, ...) case n: world.cbi.name = true; __VA_ARGS__; break
   Case(1, hasupgr1, world.cbi.perf = 30);
   Case(2, hasupgr2, world.cbi.perf = 70);
   Case(3, armorinter);
   Case(4, weapninter);
   Case(5, weapninte2);
   Case(6, rdistinter);
   #undef Case
   }
}

//
// SpawnBoss
//
[[__call("ScriptS")]]
static void SpawnBoss()
{
   ACS_Delay(1); // Delay another tic for monster spawners.
   
   Lith_ForPlayer()
   {
      if(p->active)
      {
         extern void Lith_SpawnBosses(score_t sum);
         
         Lith_SpawnBosses(p->scoresum);
         break;
      }
   }
}

#if 0
//
// Lith_MonsterInfo
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_MonsterInfo()
{
   static __str names[] = {
      "ZombieMan",
      "ShotgunGuy",
      "ChaingunGuy",
      "DoomImp",
      "Demon",
      "Spectre",
      "LostSoul",
      "Fatso",
      "Arachnotron",
      "Cacodemon",
      "HellKnight",
      "Revenant",
      "PainElemental",
      "Archvile",
      "SpiderMastermind",
      "Cyberdemon"
   };
   
   __str cname = GetActorClass(0);
   
   for(int i = 0; i < countof(names); i++)
   {
      [[__call("ScriptS")]]
      extern void Lith_MonsterMain(void);
      
      if(strstr_str(cname, names[i]))
      {
         ifauto(dmon_t, m, AllocDmon())
            Lith_MonsterMain(m);
         return;
      }
   }
}

//
// SetupMonsterInfo
//
[[__call("ScriptS")]]
static void SetupMonsterInfo()
{
   while(!enemiesarecompatible)
      ACS_Delay(1);
   
   ACS_SpawnForced("Lith_MonsterInfoEmitter", 0, 0, 0);
}
#endif

//
// DoRain
//
[[__call("ScriptS")]]
static void DoRain()
{
   // Doesn't work in multiplayer, sorry!
   if(ACS_PlayerCount() > 1)
      return;
   
   player_t *p = &players[0];
   ACS_SetActivator(p->tid);
   
   bool wasundersky = false;
   bool undersky;
   for(;;)
   {
      undersky = ACS_CheckActorCeilingTexture(0, "F_SKY1");
      
      if(undersky)
         ACS_TakeInventory("Lith_SMGHeat", 1);
      
      for(int r = 0; !undersky && r < 8; r++)
         for(int h = 1; !undersky && h <= 2; h++)
      {
         int rad = 64 << r;
         int x   = p->x + ACS_Cos(p->yaw) * rad;
         int y   = p->y + ACS_Sin(p->yaw) * rad;
         int z   = p->z + 64 / h;
         
         int tid = ACS_UniqueTID();
         ACS_SpawnForced("Lith_CameraHax", x, y, z, tid);
         
         undersky = (ACS_CheckSight(0, tid, 0) && ACS_CheckActorCeilingTexture(tid, "F_SKY1"));
         
         ACS_Thing_Remove(tid);
      }
      
      if(undersky)
      {
         if(!wasundersky)
         {
            ACS_PlaySound(p->weathertid, "amb/wind", CHAN_BODY,  1.0, true, ATTN_NONE);
            ACS_PlaySound(p->weathertid, "amb/rain", CHAN_VOICE, 1.0, true, ATTN_NONE);
         }
         
         if(world.mapscleared >= 20 && !world.islithmap)
            ACS_GiveActorInventory(p->tid, "Lith_SpawnBloodRain", 1);
         else
            ACS_GiveActorInventory(p->tid, "Lith_SpawnRain", 1);
      }
      else if(wasundersky)
      {
         ACS_PlaySound(p->weathertid, "amb/windout", CHAN_BODY,  1.0, false, ATTN_NONE);
         ACS_PlaySound(p->weathertid, "amb/rainout", CHAN_VOICE, 1.0, false, ATTN_NONE);
      }
      
      ACS_Delay(1);
      
      wasundersky = undersky;
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
   
   if(world.mapnum == 1911777)
   {
      ACS_Exit_Normal(0);
      return;
   }
   
   // Init global/static state. Check compatibility first.
   __with(int tid;)
   {
      if((world.legendoom = ACS_SpawnForced("LDLegendaryMonsterMarker", 0, 0, 0, tid = ACS_UniqueTID(), 0)))
         ACS_Thing_Remove(tid);
      if((world.grafZoneEntered = ACS_SpawnForced("Lith_GrafZone", 0, 0, 0, tid = ACS_UniqueTID(), 0)))
         ACS_Thing_Remove(tid);
   }
   
   world.drlamonsters = ACS_GetCVar("DRLA_is_using_monsters");
   
   {
      extern void Lith_GInit_Shop();
      
      Lith_GInit_Shop();
   }
   
   if(!gsinit)
   {
      extern void Lith_GSInit_Upgrade(void);
      extern void Lith_GSInit_Weapon(void);
      extern void Lith_GSInit_Dialogue(void);
      
      Lith_GSInit_Upgrade();
      Lith_GSInit_Weapon();
      Lith_GSInit_Dialogue();
      
      Lith_CheckIfEnemiesAreCompatible();
         
      world.game         = ACS_GetCVar("__lith_game");
      world.scoregolf    = ACS_GetCVar("lith_sv_scoregolf");
      world.singleplayer = ACS_GameType() == GAME_SINGLE_PLAYER;
      
      world.cbi.perf = 10;
      if(ACS_GetCVar("lith_sv_nobosses") || ACS_GetCVar("__lith_debug_level"))
         for(int i = 1; i < 7; i++)
            Lith_InstallCBIItem(i);
      
      gsinit = true;
   }
   else
   {
      extern void Lith_GSReinit_Upgrade();
      
      Lith_GSReinit_Upgrade();
   }
   
   // Map init.
   extern void Lith_LoadMapDialogue(void);
   Lith_LoadMapDialogue();
   
   world.islithmap    = (world.mapnum & 0xFFFFFC00) == 0x01202000;
   world.pauseinmenus = ACS_GetCVar("lith_sv_pauseinmenus");
   
   // Init a random seed from the map.
   world.mapseed = ACS_Random(0, 0x7FFFFFFF);
   
   // Init global score multiplier per-map.
   world.scoremul = round(ACS_GetCVarFixed("lith_sv_scoremul") * 10) / 10;
   
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
   
   // World-static pre-player init.
   bool doworldinit = false;
   
   if(ACS_Timer() <= 2)
   {
      world.bossspawned = false;
      
      if(world.unloaded)
         world.mapscleared++;
      
      if(ACS_GetCVar("lith_sv_sky") && !world.islithmap)
      {
              if(world.mapscleared >= 20) {ACS_ChangeSky("LITHSKRD", "LITHSKRD"); ACS_SetSkyScrollSpeed(1, 0.01);}
         else if(world.mapscleared >= 10)  ACS_ChangeSky("LITHSKDE", "LITHSKDE");
         else                              ACS_ChangeSky("LITHSKS1", "LITHSKS1");
      }
      
      doworldinit = true;
   }
   
   // World unloaded flag can be reset now.
   world.unloaded = false;
   
   // Sigil for when Lith_PlayerEntry can run.
   mapinit = true;
   
   // Delay so we can make sure players are initialized.
   ACS_Delay(1);
   
   // World-static post-player init.
   if(doworldinit)
   {
      if(!ACS_GetCVar("lith_sv_nobosses"))
         SpawnBoss();
      
      //SetupMonsterInfo();
      
      // Payout, which is not done on the first map.
      if(world.mapscleared != 0)
         Lith_DoPayout();
      
      // Cluster messages.
      if(world.game == Game_Doom2 && world.cluster != world.prevcluster)
      {
         switch(world.prevcluster)
         {
         case 5: Lith_ForPlayer() p->deliverMail("Cluster1"); break;
         case 6: Lith_ForPlayer() p->deliverMail("Cluster2"); break;
         case 7: Lith_ForPlayer() p->deliverMail("Cluster3"); break;
         }
      }
      
      world.prevcluster = world.cluster;
   }
   
   payout.killmax += world.mapkillmax;
   payout.itemmax += world.mapitemmax;
   
   int prevsecrets = 0;
   int prevkills   = 0;
   int previtems   = 0;
   
   // Line 1888300 is used as a control line for mod features.
   if((ACS_GetCVar("lith_sv_rain") || world.mapnum == 18883000) && !ACS_GetLineUDMFInt(1888300, "user_lith_norain"))
      DoRain();
   
   for(;;)
   {
      if(world.ticks > 17 * 35 * 60 * 60 && !world.islithmap)
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
   world.unloaded = true;
   
   for(int i = 0; i < upgradesspawnediter; i++)
      Lith_InstallCBIItem(upgradesspawned[i]);
   
   Lith_ForPlayer()
   {
      ACS_SetActivator(p->tid);
      Lith_PlayerDeinitUpgrades(p);
      p->closeGUI();
      Lith_PlayerDeltaStats(p);
   }
}

// EOF

