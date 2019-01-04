// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "w_monster.h"
#include "m_version.h"

// Extern Objects ------------------------------------------------------------|

__addrdef __mod_arr lmvar;
__addrdef __hub_arr lwvar;

bool lmvar mapinit;
i32  lmvar mapid;

struct worldinfo world = {.apiversion = Lith_APIVersion};
struct payoutinfo payout;

bool dorain;

// Static Objects ------------------------------------------------------------|

static bool reopen;

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
void Lith_SpawnBosses(i96 sum, bool force);

struct worldinfo *Lith_GetWorldExtern(void)
{
   return &world;
}

i32 Lith_UniqueID(i32 tid)
{
   i32 pn;

   // Negative values are for players.
   if((pn = Lith_GetPlayerNumber(tid)) != -1) return -(pn + 1);

   // If we already have a unique identifier, return that.
   i32 id = Lith_CheckActorInventory(tid, so_UniqueID);

   // Otherwise we have to give a new unique identifier.
   if(id == 0) Lith_GiveActorInventory(tid, so_UniqueID, id = ++mapid);

   return id;
}

// Static Functions ----------------------------------------------------------|

#if LITHIUM
script
static void CheckEnemyCompat(void)
{
   if(ACS_GetCVar(sc_sv_nomonsters) || world.enemycheck)
      return;

   i32 tid;
   if(ACS_SpawnForced(so_ZombieMan, 0, 0, 0, tid = ACS_UniqueTID(), 0))
   {
      ACS_SetActivator(tid);
      InvGive(so_EnemyChecker, 1);

      str cl = ACS_GetActorClass(0);

      LogDebug(log_dev, "Enemy check on %S", cl);

      if(strstr_str(cl, s_OBJ) || ACS_StrCmp(cl, s_RLFormer, 8) == 0)
         world.enemycompat = true;

      if(ServCallI(sm_IsHeretic) || ServCallI(sm_IsChex))
         world.enemycompat = false;

      if(world.enemycompat) LogDebug(log_dev, "Enemies are \Cdcompatible");
      else                  LogDebug(log_dev, "Enemies are \Cgnot compatible");

      world.enemycheck = true;
      ACS_Thing_Remove(0);
   }
}

script
static void SpawnBoss()
{
   ACS_Delay(1); // Delay another tic for monster spawners.

   Lith_ForPlayer() {
      Lith_SpawnBosses(p->scoresum, false);
      break;
   }
}

static void CheckModCompat(void)
{
   i32 tid;

   if((world.legendoom = ACS_SpawnForced(so_LegendaryMonsterMarker, 0, 0, 0, tid = ACS_UniqueTID(), 0))) ACS_Thing_Remove(tid);

   world.drlamonsters = ACS_GetCVar(sc_drla_is_using_monsters);
}

static void UpdateGame(void)
{
   #define Update(n) \
      if(ACS_GetCVarFixed(sc_version) < n) \
         __with(ACS_SetCVarFixed(sc_version, n);)

   Update(Lith_v1_5_1)
      ACS_SetCVarFixed(sc_sv_scoremul, 1.25); // 2.0 => 1.25

   Update(Lith_v1_5_2)
      ACS_SetCVar(sc_sv_difficulty, 10); // 1 => 10

   Update(Lith_v1_6_0)
   {
      Lith_ForPlayer()
      {
         p->setCVarK(sc_player_footstepvol, 0.2); // 1.0 => 0.2
         p->setCVarI(sc_player_ammolog, true); // false => true
      }
   }

   Update(Lith_v1_6_1)
   {
      Lith_ForPlayer()
      {
         p->setCVarK(sc_weapons_zoomfactor, 1.5); // 3.0 => 1.5
      }
   }
   #undef Update
}
#endif

static void GetDebugInfo(void)
{
   bool all = ACS_GetCVar(sc_debug_all);

   world.dbgLevel =        ACS_GetCVar(sc_debug_level);
   world.dbgItems = all || ACS_GetCVar(sc_debug_items);
   world.dbgBIP   = all || ACS_GetCVar(sc_debug_bip);
   world.dbgScore = all || ACS_GetCVar(sc_debug_score);
   world.dbgUpgr  = all || ACS_GetCVar(sc_debug_upgrades);
   world.dbgSave  = all || ACS_GetCVar(sc_debug_save);
   world.dbgNoMon =        ACS_GetCVar(sc_debug_nomonsters);
}

static void GSInit(void)
{
   extern void Lith_GInit_Shop(void);
   extern void Lith_GSReinit_Upgrade(void);
   extern void Lith_GSInit_Upgrade(void);
   extern void Lith_GSInit_Weapon(void);
   extern void Lith_GSInit_Dialogue(void);

   LogDebug(log_dev, "GINIT RUNNING");

   GetDebugInfo();
   #if LITHIUM
   CheckModCompat();
   UpdateGame();
   Lith_GInit_Shop();
   #endif

   if(!world.gsinit)
   {
      LogDebug(log_dev, "GSINIT RUNNING");

      Lith_GSInit_Upgrade();
      #if LITHIUM
      Lith_GSInit_Weapon();
      Lith_GSInit_Dialogue();

      CheckEnemyCompat();
      #endif

      world.game         = ACS_GetCVar(sc_game);
      world.singleplayer = ACS_GameType() == GAME_SINGLE_PLAYER;

      world.cbiperf = 10;

      world.gsinit = true;
   }
   else
      Lith_GSReinit_Upgrade();
}

static void MInit(void)
{
   LogDebug(log_dev, "MINIT RUNNING");

   #if LITHIUM
   extern void Lith_LoadMapDialogue(void);
   Lith_LoadMapDialogue();

   world.islithmap = (world.mapnum & 0xFFFFFC00) == 0x01202000;

   world.pauseinmenus = world.singleplayer && ACS_GetCVar(sc_sv_pauseinmenus);

   world.soulsfreed = 0;
   #endif

   // Init a random seed from the map.
   world.mapseed = ACS_Random(0, 0x7FFFFFFF);

   // Init global score multiplier per-map.
   world.scoremul = roundlk(ACS_GetCVarFixed(sc_sv_scoremul) * 10, 10) / 10;

   // Give players some extra score if they're playing on extra hard or above.
   if(ACS_GameSkill() >= skill_extrahard)
      world.scoremul += 0.15;

   // Set the air control because ZDoom's default sucks.
   ACS_SetAirControl(0.77);
}

static void MSInit(void)
{
   script
   extern void Lith_DoRain();

   LogDebug(log_dev, "MSINIT RUNNING");

   #if LITHIUM
   payout.killmax += world.mapkillmax;
   payout.itemmax += world.mapitemmax;

   // Line 1888300 is used as a control line for mod features.
   // Check for if rain should be used.
   if(!ACS_GetLineUDMFInt(1888300, sm_MapNoRain) &&
      (ACS_GetCVar(sc_sv_rain) || ACS_GetLineUDMFInt(1888300, sm_MapUseRain)) &&
      ACS_PlayerCount() <= 1)
   {
      dorain = true;
      Lith_DoRain();
   }
   #endif
}

static void WSInit(void)
{
   extern void DmonInit();

   LogDebug(log_dev, "WSINIT RUNNING");

   if(world.unloaded)
      world.mapscleared++;

   #if LITHIUM
   DmonInit();
   world.bossspawned = false;

   if(ACS_GetCVar(sc_sv_sky) && !world.islithmap)
   {
      if(InHell)
      {
         ACS_ChangeSky(s_LITHSKRD, s_LITHSKRD);
         ACS_SetSkyScrollSpeed(1, 0.01);
      }
      else if(OnEarth)
         ACS_ChangeSky(s_LITHSKDE, s_LITHSKDE);
      else
         ACS_ChangeSky(s_LITHSKS1, s_LITHSKS1);
   }
   #endif
}

static void WInit(void)
{
   extern void Lith_DoPayout(void);

   LogDebug(log_dev, "WINIT RUNNING");

   #if LITHIUM
   if(!ACS_GetCVar(sc_sv_nobosses))
      SpawnBoss();

   // Payout, which is not done on the first map.
   if(world.mapscleared != 0)
      Lith_DoPayout();
   #endif

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
      Message(>= 6,  0, st_mail_cluster1);
      Message(>= 7,  1, st_mail_cluster2);
      Message(== 8,  2, st_mail_cluster3);
      Message(== 9,  3, st_mail_secret1);
      Message(== 10, 4, st_mail_secret2);
   }

   #if LITHIUM
   if(ACS_GetCVar(sc_sv_nobosses) || world.dbgItems)
      for(i32 i = 0; i < cupg_max; i++)
         Lith_InstallCBIItem(i);
   #endif
}

script ext("ACS")
void Lith_Finale(void)
{
   ServCallI(sm_SetEnding, st_normal);
   ACS_ChangeLevel(s_LITHEND, 0, CHANGELEVEL_NOINTERMISSION|CHANGELEVEL_PRERAISEWEAPON, -1);
}

// Scripts -------------------------------------------------------------------|

script type("open")
static void Lith_World(void)
{
begin:
   #if LITHIUM
   if(ACS_GameType() == GAME_TITLE_MAP) {
      script
      extern void Lith_Title(void);
      Lith_Title();
      return;
   }
   #endif

   LogDebug(log_dev, "LITH OPEN");

   if(ACS_GetCVar(sc_sv_failtime) == 0) for(;;)
   {
      Log("\n=======\n"
          "The configuration for this mod has been wiped, or you accidentally "
          "set '" CVAR "sv_failtime' to 0 manually. If you did the latter, "
          "please set it to something else. Otherwise, please follow these "
          "instructions to fix your configuration:\n"
          "\n"
          "1. Navigate to your GZDoom folder.\n"
          "2. Find the configuration settings file (if you have extensions "
          "shown it will be the ini file in the folder) and open it.\n"
          "3. Find the heading '[Doom.Player.Mod]' and delete any lines "
          "starting with '" CVAR "' or '" DCVAR "' under it.\n"
          "4. Find the heading '[Doom.LocalServerInfo.Mod]' and delete any "
          "lines starting with '" CVAR "' or '" DCVAR "' under it.\n"
          "5. Save the file and start GZDoom again. If the issue persists "
          "try these steps again or delete your GZDoom configuration.\n"
          "\n\n\n\n");
      Log("Invalid settings detected. Please open the console (%jS or options menu) for more information.", "toggleconsole");
      ACS_Delay(10);
   }

   if(world.mapnum == 1911777)
   {
      ACS_SetPlayerProperty(true, true, PROP_TOTALLYFROZEN);
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
   i32 prevsecrets = 0;
   i32 prevkills   = 0;
   i32 previtems   = 0;

   i32 missionkill = 0;
   i32 missionprc  = 0;

   for(;;)
   {
      if(reopen) {
         mapinit = reopen = false;
         goto begin;
      }

      #if LITHIUM
      if(world.ticks > ACS_GetCVar(sc_sv_failtime) * 35 * 60 * 60 && !world.islithmap)
      {
         ServCallI(sm_SetEnding, st_timeout);
         ACS_ChangeLevel(s_LITHEND, 0, CHANGELEVEL_NOINTERMISSION, -1);
         return;
      }
      #endif

      i32 secrets = world.mapsecrets;
      i32 kills   = world.mapkills;
      i32 items   = world.mapitems;

      if(secrets > prevsecrets) {
         i32 delta = secrets - prevsecrets;
         Lith_GiveAllScore(9000 * delta, true);
         world.secretsfound += delta;
      }

      if(kills > prevkills) payout.killnum += kills - prevkills;
      if(items > previtems) payout.itemnum += items - previtems;

      prevsecrets = secrets;
      prevkills   = kills;
      previtems   = items;

      #if LITHIUM
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
      #endif

      ACS_Delay(1);

      dbgstatnum = 0;
      world.ticks++;

      if(world.autosave && world.ticks % (35 * 60 * world.autosave) == 0)
         ACS_Autosave();
   }
}

script ext("ACS")
void Lith_WorldReopen(void)
{
   reopen = true;
}

script type("unloading")
static void Lith_WorldUnload(void)
{
   extern void Lith_InstallSpawnedCBIItems(void);
   world.unloaded = true;
   LogDebug(log_dev, "WORLD UNLOADED");

   #if LITHIUM
   Lith_InstallSpawnedCBIItems();
   #endif

   Lith_ForPlayer() {
      p->setActivator();
      Lith_PlayerDeinitUpgrades(p);
      p->closeGUI();
      Lith_PlayerUpdateStats(p);
   }
}

// EOF
