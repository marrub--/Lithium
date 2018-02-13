// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"
#include "lith_monster.h"
#include "lith_version.h"

#include <math.h>

#define InSecret \
   (world.game == Game_Doom2 && (world.cluster == 9 || world.cluster == 10))

#define InHell \
   (world.game == Game_Doom2 && world.cluster >= 8)

#define OnEarth \
   (world.game == Game_Doom2 && world.cluster == 7)

// Extern Objects ------------------------------------------------------------|

__addrdef __mod_arr lmvar;
__addrdef __hub_arr lwvar;

bool lmvar mapinit;
int  lmvar mapid;

worldinfo_t world = {.apiversion = Lith_APIVersion};
payoutinfo_t payout;

// Static Objects ------------------------------------------------------------|

static bool  lmvar rain_chk;
static fixed lmvar rain_px;
static fixed lmvar rain_py;
static int   lmvar rain_dist;

static bool reopen;
static bool dorain;

// Extern Functions ----------------------------------------------------------|

[[__call("ScriptS"), __extern("ACS")]]
void Lith_SpawnBosses(score_t sum, bool force);

//
// Lith_GetWorldExtern
//
worldinfo_t *Lith_GetWorldExtern(void)
{
   return &world;
}

//
// Lith_FreezeTime
//
[[__call("StkCall")]]
void Lith_FreezeTime(bool on)
{
   static int lmvar frozen;

   if(on)
   {
      if(!frozen++)
      {
         Lith_ForPlayer()
         {
            p->frozen++;
            p->setVel(0, 0, 0);
         }

         Lith_ForPlayer()
         {
            Lith_GiveActorInventory(p->tid, "Lith_TimeHax", 1);
            Lith_GiveActorInventory(p->tid, "Lith_TimeHax2", 1);
            break;
         }
      }
   }
   else
   {
      if(!--frozen)
      {
         Lith_ForPlayer() p->frozen--;

         Lith_ForPlayer()
         {
            Lith_TakeActorInventory(p->tid, "PowerTimeFreezer", 1);
            Lith_TakeActorInventory(p->tid, "Lith_TimeHax2", 1);
            break;
         }
      }
   }
}

//
// Lith_BeginAngles
//
[[__call("StkCall")]]
void Lith_BeginAngles(int x, int y)
{
   world.a_cur = 0;
   for(int i = 0; i < countof(world.a_angles); i++)
   {
      world.a_angles[i].ang = 0;
      world.a_angles[i].dst = 0;
   }
   world.a_x = x;
   world.a_y = y;
}

//
// Lith_AddAngle
//
[[__call("StkCall")]]
fixed Lith_AddAngle(int x, int y)
{
   if(world.a_cur >= countof(world.a_angles))
      return 0;

   struct polar *p = &world.a_angles[world.a_cur++];
   *p = ctopol(x - world.a_x, y - world.a_y);
   return p->ang;
}

//
// Lith_AddAngleScript
//
[[__call("ScriptS"), __extern("ACS"), __address("Lith_AddAngle")]]
void Lith_AddAngleScript(int x, int y)
{
   Lith_AddAngle(x, y);
}

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
   Lith_GiveAllScore(amount, false);
}

//
// Lith_EmitEXP
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_EmitEXP(int amount)
{
   Lith_GiveAllEXP(amount);
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
// LPData
//
[[__call("ScriptS"), __extern("ACS")]]
int LPData(int info, int permutation, bool target)
{
   if(target) ACS_SetActivatorToTarget(0);

   withplayer(LocalPlayer)
      switch(info)
   {
   case pdata_upgrade:    return p->getUpgrActive(permutation);
   case pdata_riflemode:  return p->riflefiremode;
   case pdata_hassigil:   return p->sigil.acquired;
   case pdata_weaponzoom: return bitsk(p->getCVarK("lith_weapons_zoomfactor"));
   case pdata_recoil:     return bitsk(p->getCVarK("lith_weapons_recoil"));
   case pdata_slot3ammo:  return p->getCVarI("lith_weapons_slot3ammo");
   case pdata_pclass:     return p->pclass;
   case pdata_semifrozen: return p->semifrozen > 0;
   }

   return 0;
}

//
// LWData
//
[[__call("ScriptS"), __extern("ACS")]]
int LWData(int info)
{
   switch(info)
   {
   case wdata_brightweps:  return ACS_GetUserCVar(0, "lith_player_brightweps");
   case wdata_noitemfx:    return ACS_GetUserCVar(0, "lith_player_noitemfx");
   case wdata_magdrops:    return ACS_GetUserCVar(0, "lith_weapons_magdrops");
   case wdata_bossspawned: return world.bossspawned;
   case wdata_enemycheck:  return world.enemycheck;
   case wdata_fun:         return world.fun;
   case wdata_soulsfreed:  return world.soulsfreed;
   case wdata_dorain:      return dorain;
   case wdata_ptid:   Lith_ForPlayer() return p->tid;
   case wdata_pclass: Lith_ForPlayer() return p->pclass;
   }

   return 0;
}

//
// Lith_RainDropSpawn
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_RainDropSpawn()
{
   if(rain_chk)
   {
      int dist =
         mag2i(ACS_GetActorX(0) - rain_px, ACS_GetActorY(0) - rain_py);
      if(dist < rain_dist)
         rain_dist = dist;
   }
}

// Static Functions ----------------------------------------------------------|

//
// DoRain
//
[[__call("ScriptS")]]
static void DoRain()
{
   if(ACS_PlayerCount() > 1)
      return;

   dorain = true;

   player_t *p = &players[0];
   p->setActivator();

   ACS_PlaySound(p->weathertid, "amb/wind", CHAN_BODY,  0.001, true, ATTN_NONE);
   ACS_PlaySound(p->weathertid, "amb/rain", CHAN_VOICE, 0.001, true, ATTN_NONE);

   fixed skydist, curskydist = 1;
   for(;;)
   {
      if((rain_chk = !ACS_CheckActorCeilingTexture(0, "F_SKY1")))
      {
         rain_dist = 1024;
         rain_px = p->x;
         rain_py = p->y;
      }
      else
         InvTake("Lith_SMGHeat", 1);

      if((InHell || InSecret) && !world.islithmap)
         HERMES("SpawnRain", "Lith_BloodRainDrop");
      else
         HERMES("SpawnRain", "Lith_RainDrop");

      ACS_Delay(1);

      if(rain_chk)
      {
         skydist = rain_dist / 1024.0;
         skydist = minmax(skydist, 0, 1);
      }
      else
         skydist = 0;

      curskydist = lerpk(curskydist, skydist, 0.035);
      ACS_SoundVolume(p->weathertid, CHAN_BODY,  1 - curskydist);
      ACS_SoundVolume(p->weathertid, CHAN_VOICE, 1 - curskydist);
   }
}

//
// DoPayout
//
static void DoPayout(void)
{
   fixed64_t taxpct = ACS_RandomFixed(0, 4 / 100.0);

   #define GenPay(name) \
      if(payout.name##max) \
      { \
         payout.name##pct = (payout.name##num / (fixed64_t)payout.name##max) * 100; \
         payout.name##scr = payout.name##pct * 600; \
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

   payout = (payoutinfo_t){};
}

//
// CheckEnemyCompat
//
[[__call("ScriptS")]]
static void CheckEnemyCompat(void)
{
   if(ACS_GetCVar("sv_nomonsters") || world.enemycheck)
      return;

   int tid;
   if(ACS_SpawnForced("ZombieMan", 0, 0, 0, tid = ACS_UniqueTID(), 0))
   {
      ACS_SetActivator(tid);
      InvGive("Lith_EnemyChecker", 1);

      __str cl = ACS_GetActorClass(0);

      LogDebug(log_dev, "Enemy check on %S", cl);

      if(strstr_str(cl, "Lith_") || ACS_StrCmp(cl, "RLFormer", 8) == 0)
         world.enemycompat = true;

      if(world.enemycompat) LogDebug(log_dev, "Enemies are \Cdcompatible");
      else                  LogDebug(log_dev, "Enemies are \Cgnot compatible");

      world.enemycheck = true;
      ACS_Thing_Remove(0);
   }
}

//
// SpawnBoss
//
[[__call("ScriptS")]]
static void SpawnBoss()
{
   ACS_Delay(1); // Delay another tic for monster spawners.

   Lith_ForPlayer() {
      Lith_SpawnBosses(p->scoresum, false);
      break;
   }
}

//
// GetDebugInfo
//
static void GetDebugInfo(void)
{
   bool all = ACS_GetCVar("__lith_debug_all");

   world.dbgLevel =        ACS_GetCVar("__lith_debug_level");
   world.dbgItems = all || ACS_GetCVar("__lith_debug_items");
   world.dbgBIP   = all || ACS_GetCVar("__lith_debug_bip");
   world.dbgScore = all || ACS_GetCVar("__lith_debug_score");
   world.dbgUpgr  = all || ACS_GetCVar("__lith_debug_upgrades");
   world.dbgSave  = all || ACS_GetCVar("__lith_debug_save");
   world.dbgNoMon =        ACS_GetCVar("__lith_debug_nomonsters");
}

//
// CheckModCompat
//
static void CheckModCompat(void)
{
   int tid;

   if((world.legendoom = ACS_SpawnForced("LDLegendaryMonsterMarker", 0, 0, 0, tid = ACS_UniqueTID(), 0))) ACS_Thing_Remove(tid);

   world.drlamonsters = ACS_GetCVar("DRLA_is_using_monsters");
}

//
// UpdateGame
//
static void UpdateGame(void)
{
   #define Update(n) \
      if(ACS_GetCVarFixed("__lith_version") < n) \
         __with(ACS_SetCVarFixed("__lith_version", n);)

   Update(15.1) ACS_SetCVarFixed("lith_sv_scoremul", 1.25); // 2.0=>1.25
   Update(15.2) ACS_SetCVar("lith_sv_difficulty", 10); // 1=>10
   Update(16.0)
      Lith_ForPlayer() p->setCVarK("lith_player_footstepvol", 0.2); // 1.0=>0.2

   #undef Update
}

//
// GSInit
//
static void GSInit(void)
{
   extern void Lith_GInit_Shop(void);
   extern void Lith_GSReinit_Upgrade(void);
   extern void Lith_GSInit_Upgrade(void);
   extern void Lith_GSInit_Weapon(void);
   extern void Lith_GSInit_Dialogue(void);
   extern void Lith_InstallCBIItem(int num);

   LogDebug(log_dev, "GINIT RUNNING");

   CheckModCompat();
   UpdateGame();
   GetDebugInfo();
   Lith_GInit_Shop();

   if(!world.gsinit)
   {
      LogDebug(log_dev, "GSINIT RUNNING");

      Lith_GSInit_Upgrade();
      Lith_GSInit_Weapon();
      Lith_GSInit_Dialogue();

      CheckEnemyCompat();

      world.game         = ACS_GetCVar("__lith_game");
      world.singleplayer = ACS_GameType() == GAME_SINGLE_PLAYER;

      world.cbiperf = 10;
      if(ACS_GetCVar("lith_sv_nobosses") || world.dbgItems)
         for(int i = 0; i < cupg_max; i++)
            Lith_InstallCBIItem(i);

      world.gsinit = true;
   }
   else
      Lith_GSReinit_Upgrade();
}

//
// MInit
//
static void MInit(void)
{
   LogDebug(log_dev, "MINIT RUNNING");

   extern void Lith_LoadMapDialogue(void);
   Lith_LoadMapDialogue();

   world.islithmap    = (world.mapnum & 0xFFFFFC00) == 0x01202000;
   world.pauseinmenus = world.singleplayer && ACS_GetCVar("lith_sv_pauseinmenus");

   world.soulsfreed = 0;

   // Init a random seed from the map.
   world.mapseed = ACS_Random(0, 0x7FFFFFFF);

   // Init global score multiplier per-map.
   world.scoremul = round(ACS_GetCVarFixed("lith_sv_scoremul") * 10) / 10;

   // Give players some extra score if they're playing on extra hard or above.
   if(ACS_GameSkill() >= skill_extrahard)
      world.scoremul += 0.15;

   // Set the air control because ZDoom's default sucks.
   ACS_SetAirControl(0.77);

   // Set up translations.
   for(int i = 0; i < 8; i++)
   {
      static struct {int r,g,b;} const trs[8] = {
         {0xF5,0x22,0x22},
         {0xF5,0x22,0xC4},
         {0xC0,0x22,0xF5},
         {0x50,0x22,0xF5},
         {0x22,0xF5,0xF5},
         {0x22,0xF5,0x8C},
         {0xF5,0xF5,0x22},
         {0xF5,0xB2,0x22},
      };
      ACS_CreateTranslationStart(42470 + i);
      ACS_CreateTranslationRGB(80, 111, trs[i].r,   trs[i].g,   trs[i].b,
                                        trs[i].r/4, trs[i].g/4, trs[i].b/4);
      ACS_CreateTranslationEnd();
   }
}

//
// MSInit
//
static void MSInit(void)
{
   LogDebug(log_dev, "MSINIT RUNNING");

   payout.killmax += world.mapkillmax;
   payout.itemmax += world.mapitemmax;

   // Line 1888300 is used as a control line for mod features.
   // Check for if rain should be used.
   if(!ACS_GetLineUDMFInt(1888300, "user_lith_norain") &&
      (ACS_GetCVar("lith_sv_rain") || ACS_GetLineUDMFInt(1888300, "user_lith_userain")))
      DoRain();
}

//
// WSInit
//
static void WSInit(void)
{
   LogDebug(log_dev, "WSINIT RUNNING");

   dmonid = 0;
   world.bossspawned = false;

   if(world.unloaded)
      world.mapscleared++;

   if(ACS_GetCVar("lith_sv_sky") && !world.islithmap)
   {
      if(InHell)
      {
         ACS_ChangeSky("LITHSKRD", "LITHSKRD");
         ACS_SetSkyScrollSpeed(1, 0.01);
      }
      else if(OnEarth)
         ACS_ChangeSky("LITHSKDE", "LITHSKDE");
      else
         ACS_ChangeSky("LITHSKS1", "LITHSKS1");
   }
}

//
// WInit
//
static void WInit(void)
{
   LogDebug(log_dev, "WINIT RUNNING");

   if(!ACS_GetCVar("lith_sv_nobosses"))
      SpawnBoss();

   // Payout, which is not done on the first map.
   if(world.mapscleared != 0)
      DoPayout();

   // Cluster messages.
   #define Message(cmp, n, name) \
      if(world.cluster cmp && !msgs[n]) { \
         Lith_ForPlayer() \
            p->deliverMail(name); \
         msgs[n] = true; \
      } else ((void)0)

   static bool msgs[5];

   if(world.game == Game_Doom2)
   {
      Message(>= 6,  0, "Cluster1");
      Message(>= 7,  1, "Cluster2");
      Message(== 8,  2, "Cluster3");
      Message(== 9,  3, "Secret1");
      Message(== 10, 4, "Secret2");
   }
}

// Scripts -------------------------------------------------------------------|

//
// Lith_World
//
[[__call("ScriptS"), __script("Open")]]
static void Lith_World(void)
{
begin:
   if(ACS_GameType() == GAME_TITLE_MAP) {
      [[__call("ScriptS")]]
      extern void Lith_Title(void);
      Lith_Title();
      return;
   }

   LogDebug(log_dev, "LITH OPEN");

   if(world.mapnum == 1911777)
   {
      ACS_Exit_Normal(0);
      return;
   }

   dbgnotenum = 0;

   ACS_Delay(1);

   GSInit(); // Init global state.
   MInit();  // Map init.

   // World-static pre-player init.
   bool doworldinit = false;

   if(ACS_Timer() <= 2)
   {
      WSInit();
      doworldinit = true;
   }

   world.unloaded = false; // World unloaded flag can be reset now.
   mapinit = true;         // Sigil for when Lith_PlayerEntry can run.

   ACS_Delay(1); // Delay so players get initialized.

   // World-static post-player init.
   if(doworldinit)
      WInit();

   // Map-static post-world init.
   MSInit();

   // Main loop.
   int prevsecrets = 0;
   int prevkills   = 0;
   int previtems   = 0;

   int missionkill = 0;
   int missionprc  = 0;

   for(;;)
   {
      if(reopen) {
         mapinit = reopen = false;
         goto begin;
      }

      if(world.ticks > 67 * 35 * 60 * 60 && !world.islithmap)
         ACS_Teleport_NewMap(1911777, 0, 0);

      int secrets = world.mapsecrets;
      int kills   = world.mapkills;
      int items   = world.mapitems;

      if(secrets > prevsecrets) {
         int delta = secrets - prevsecrets;
         Lith_GiveAllScore(9000 * delta, true);
         world.secretsfound += delta;
      }

      if(kills > prevkills) payout.killnum += kills - prevkills;
      if(items > previtems) payout.itemnum += items - previtems;

      prevsecrets = secrets;
      prevkills   = kills;
      previtems   = items;

      if(world.enemycheck && !world.dbgNoMon) {
         extern void DmonDebugInfo(void);
         DmonDebugInfo();
      }

      if(ACS_Timer() % 5 == 0 && missionkill < kills)
      {
         if(++missionprc >= 150) {
            Lith_SpawnBosses(0, true);
            missionprc = 0;
         }
         missionkill = kills;
      }

      DebugStat("mission%%: %i\n", missionprc);

      ACS_Delay(1);

      dbgstatnum = 0;
      world.ticks++;

      if(world.autosave && world.ticks % (35 * 60 * world.autosave) == 0)
         ACS_Autosave();
   }
}

//
// Lith_WorldReopen
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_WorldReopen(void)
{
   reopen = true;
}

//
// Lith_WorldUnload
//
[[__call("ScriptS"), __script("Unloading")]]
static void Lith_WorldUnload(void)
{
   extern void Lith_InstallSpawnedCBIItems(void);
   world.unloaded = true;
   LogDebug(log_dev, "WORLD UNLOADED");

   Lith_InstallSpawnedCBIItems();

   Lith_ForPlayer() {
      p->setActivator();
      Lith_PlayerDeinitUpgrades(p);
      p->closeGUI();
      Lith_PlayerUpdateStats(p);
   }
}

// EOF

