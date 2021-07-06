/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * World entry points.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "w_monster.h"
#include "m_version.h"

/* Extern Objects ---------------------------------------------------------- */

__addrdef __mod_arr lmvar;
__addrdef __hub_arr lhvar;

struct payoutinfo payout;
i32 mapscleared;
i32 prevcluster;
i32 mapseed;
bool unloaded;
bool lmvar islithmap;
i32 secretsfound;
k64 scoremul;
u64 ticks;
i32 soulsfreed;
bool bossspawned;
i32 cbiperf;
bool cbiupgr[cupg_max];
bool legendoom;
bool drlamonsters;
enum mission_status lmvar mission = _unfinished;

bool dorain;

bool lmvar player_init;

bool reopen;

i32 lmvar mapid;

bool lmvar modinit;
bool lhvar hubinit;
bool       gblinit;

/* Extern Functions -------------------------------------------------------- */

script void SpawnBosses(i96 sum, bool force);

i32 UniqueID(i32 tid) {
   i32 pn;

   /* Negative values are for players. */
   if((pn = PtrPlayerNumber(tid)) != -1) return -(pn + 1);

   /* If we already have a unique identifier, return that. */
   i32 id = PtrInvNum(tid, so_UniqueID);

   /* Otherwise we have to give a new unique identifier. */
   if(id == 0) PtrInvGive(tid, so_UniqueID, id = ++mapid);

   return id;
}

/* Static Functions -------------------------------------------------------- */

alloc_aut(0) script static
void Boss_HInit(void) {
   ACS_Delay(1); /* Delay another tic for monster spawners. */

   SpawnBosses(pl.scoresum, false);
}

static
void CheckModCompat(void) {
   i32 tid;

   if((legendoom = ACS_SpawnForced(so_LDLegendaryMonsterMarker, 0, 0, 0, tid = ACS_UniqueTID(), 0))) ACS_Thing_Remove(tid);

   drlamonsters = CVarGetI(sc_drla_is_using_monsters);
}

static
bool updateTo(k32 to) {
   k32 cur = CVarGetK(sc_version);
   if(cur < to) {
      CVarSetK(sc_version, to);
      return true;
   } else {
      return false;
   }
}

static
void UpdateGame(void) {
   if(updateTo(Ver1_5_1)) {
      CVarSetK(sc_sv_scoremul, 1.25); /* 2.0 => 1.25 */
   }

   if(updateTo(Ver1_5_2)) {
      CVarSetI(sc_sv_difficulty, 10); /* 1 => 10 */
   }

   if(updateTo(Ver1_6_0)) {
      CVarSetK(sc_player_footstepvol, 0.2); /* 1.0 => 0.2 */
      CVarSetI(sc_player_ammolog, true); /* false => true */
   }

   if(updateTo(Ver1_6_1)) {
      CVarSetK(sc_weapons_zoomfactor, 1.5); /* 3.0 => 1.5 */
   }

   if(updateTo(Ver1_7_0)) {
      /* unfortunate, but we forgot to add this for 1.6.3. so, we'll fix it in
       * version 1.7 instead.
       */
      if(CVarGetI(sc_xhair_style) >= 10) {
         CVarSetI(sc_xhair_style, 0);
      }
      /* accidentally set this to something the settings menu couldn't
       * actually use
       */
      CVarSetK(sc_sv_scoremul, 1.2); /* 1.25 => 1.2 */
   }
}

static
void MInitPre(void) {
   Dbg_Log(log_dev, "%s", __func__);

   CheckModCompat();
   UpdateGame();
}

static
void GInit(void) {
   Dbg_Log(log_dev, "%s", __func__);

   cbiperf = 10;

   islithmap = (MapNum >= LithMapBeg && MapNum <= LithMapEnd);

   Mon_Init();
   Wep_GInit();

   gblinit = true;
}

static
void MInitPst(void) {
   Dbg_Log(log_dev, "%s", __func__);

   payout.par = ACS_GetLevelInfo(LEVELINFO_PAR_TIME) * 35;

   payout.killmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_MONSTERS);
   payout.itemmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_ITEMS);
   payout.scrtmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_SECRETS);

   /* Check for if rain should be used.
    * - If there are more than 1 players, never use rain.
    * - If `NoRain' is set on LithMapLine, never use rain.
    * - If the player has rain enabled, use it if not for those preconditions.
    * - If `UseRain' is set on LithMapLine, use it if not for that.
    */
   bool multi_player  = ACS_PlayerCount() > 1;
   bool never_rain    = ACS_GetLineUDMFInt(LithMapLine, sm_MapNoRain);
   bool use_rain_user = CVarGetI(sc_sv_rain);
   bool use_rain_map  = ACS_GetLineUDMFInt(LithMapLine, sm_MapUseRain);
   if(!multi_player && !never_rain && (use_rain_user || use_rain_map)) {
      dorain = true;
      W_DoRain();
   }

   modinit = true;
}

static
void MInit(void) {
   Dbg_Log(log_dev, "%s", __func__);

   Dlg_MInit();

   soulsfreed = 0;

   /* Init a random seed from the map. */
   mapseed = ACS_Random(0, INT32_MAX);

   /* Init global score multiplier per-map. */
   scoremul = fastroundlk(CVarGetK(sc_sv_scoremul) * 10.0k) / 10.0k;

   /* Give players some extra score if they're playing on extra hard or above. */
   if(ACS_GameSkill() >= skill_extrahard)
      scoremul += 0.15;

   /* Set the air control because ZDoom's default sucks. */
   ACS_SetAirControl(0.77);

   Upgr_MInit();
   Shop_MInit();
}

static
void HInitPre(void) {
   Dbg_Log(log_dev, "%s", __func__);

   if(unloaded)
      mapscleared++;

   bossspawned = false;

   if(CVarGetI(sc_sv_sky) && !islithmap) {
      if(InHell) {
         ACS_ChangeSky(sp_LITHSKRD, sp_LITHSKRD);
         ACS_SetSkyScrollSpeed(1, 0.01);
      } else if(OnEarth) {
         ACS_ChangeSky(sp_LITHSKDE, sp_LITHSKDE);
      } else {
         ACS_ChangeSky(sp_LITHSKS1, sp_LITHSKS1);
      }
   }
}

static
void HInit(void) {
   Dbg_Log(log_dev, "%s", __func__);

   if(!CVarGetI(sc_sv_nobosses))
      Boss_HInit();

   /* Payout, which is not done on the first map. */
   if(mapscleared != 0) Scr_HInit();

   /* Cluster messages. */
   if(Cluster >=  6) P_BIP_Unlock(P_BIP_NameToPage("MCluster1"), false);
   if(Cluster >=  7) P_BIP_Unlock(P_BIP_NameToPage("MCluster2"), false);
   if(Cluster ==  8) P_BIP_Unlock(P_BIP_NameToPage("MCluster3"), false);
   if(Cluster ==  9) P_BIP_Unlock(P_BIP_NameToPage("MSecret1"),  false);
   if(Cluster == 10) P_BIP_Unlock(P_BIP_NameToPage("MSecret2"),  false);

   if(CVarGetI(sc_sv_nobosses) ||
      CVarGetI(sc_sv_nobossdrop)
      #ifndef NDEBUG
      || get_bit(dbgflags, dbgf_items)
      #endif
      )
   {
      for(i32 i = 0; i < cupg_max; i++)
         CBI_Install(i);
   }

   hubinit = true;
}

/* Scripts ----------------------------------------------------------------- */

_Noreturn dynam_aut script type("open") static
void Sc_World(void) {
   Dbg_Log(log_dev, "%s", __func__);

begin:
   /* yep. ZDoom doesn't actually clear hub variables in Doom-like map setups.
    * we can still detect it by using Timer, so correct this variable.
    */
   if(hubinit && ACS_Timer() < 2) hubinit = false;

   Draw_Init();

   if(ACS_GameType() == GAME_TITLE_MAP) {
      W_Title();
      return;
   } else if(MapNum == 1911777) {
      ACS_SetPlayerProperty(true, true, PROP_TOTALLYFROZEN);
      F_Load();
      pl.setActivator();
      F_Run();
      return;
   }

   #ifndef NDEBUG
   if(CVarGetI(sc_sv_failtime) == 0) for(;;) {
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
          "\n\n\n\n\n\n"
          "Invalid settings detected. Please open the console"
          "(\"%jS\" or options menu) for more information.",
          sc_toggleconsole);
      ACS_Delay(10);
   }

   dbgnotenum = 0;
   #endif

   /* Now, initialize everything.
    * Start by deallocating temporary tags.
    */
   Xalloc(_tag_temp);

   if(!modinit) MInitPre();
   if(!gblinit) GInit();
   if(!modinit) MInit();

   /* Hub-static pre-player init. */
   if(!hubinit) HInitPre();

   unloaded = false; /* Unloaded flag can be reset now. */
   player_init = true;

   ACS_Delay(1); /* Wait for players to get initialized. */

   /* Hub-static post-player init. */
   if(!hubinit) HInit();

   /* Module-static post-hub init. */
   if(!modinit) MInitPst();

   /* Main loop. */
   i32 prevscrts = 0;
   i32 prevkills = 0;
   i32 previtems = 0;

   i32 missionkill = 0;
   i32 missionprc  = 0;

   for(;;) {
      if(reopen) {
         player_init = reopen = false;
         goto begin;
      }

      if(ticks > CVarGetI(sc_sv_failtime) * 35 * 60 * 60 && !islithmap) {
         F_Start("TimeOut");
         return;
      }

      i32 scrts = ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS);
      i32 kills = ACS_GetLevelInfo(LEVELINFO_KILLED_MONSTERS);
      i32 items = ACS_GetLevelInfo(LEVELINFO_FOUND_ITEMS);

      if(kills > prevkills)  payout.killnum += kills - prevkills;
      if(items > previtems)  payout.itemnum += items - previtems;
      if(scrts > prevscrts) {payout.scrtnum += scrts - prevscrts;
                             secretsfound   += scrts - prevscrts;}

      prevscrts = scrts;
      prevkills = kills;
      previtems = items;

      if(ACS_Timer() % 5 == 0 && missionkill < kills) {
         if(++missionprc >= 150) {
            SpawnBosses(0, true);
            missionprc = 0;
         }
         missionkill = kills;
      }

      Dbg_Stat("mission%%: %i\n", missionprc);

      ACS_Delay(1);

      #ifndef NDEBUG
      dbgstatnum = 0;
      #endif
      ticks++;

      i32 autosave = CVarGetI(sc_sv_autosave);
      if(autosave && ticks % (35 * 60 * autosave) == 0) ACS_Autosave();
   }
}

script_str ext("ACS") addr(OBJ "InHell") i32 Sc_InHell(void) {return InHell;}

script_str ext("ACS") addr(OBJ "SkyMap") i32 Sc_SkyMap(void) {
   return CVarGetI(sc_sv_sky) && !islithmap;
}

alloc_aut(0) script ext("ACS") addr(lsc_worldreopen)
void Sc_WorldReopen(void) {
   reopen = true;
}

script type("unloading") static
void Sc_WorldUnload(void) {
   unloaded = true;
   Dbg_Log(log_dev, "%s", __func__);

   CBI_InstallSpawned();

   pl.setActivator();
   P_Upg_PDeinit();
   P_GUI_Close();
   P_Dat_PTickPst();
}

/* EOF */
