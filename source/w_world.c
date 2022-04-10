// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ World entry points.                                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "w_world.h"
#include "w_monster.h"
#include "m_version.h"

__addrdef __mod_arr lmvar;

noinit struct world            wl;
noinit struct map_locals lmvar ml;

script void SpawnBosses(i96 sum, bool force);

i32 UniqueID(i32 tid) {
   i32 pn;

   /* Negative values are for players. */
   if((pn = PtrPlayerNumber(tid)) != -1) return -(pn + 1);

   /* If we already have a unique identifier, return that. */
   i32 id = PtrInvNum(tid, so_UniqueID);

   /* Otherwise we have to give a new unique identifier. */
   if(id == 0) PtrInvGive(tid, so_UniqueID, id = ++ml.mapid);

   return id;
}

alloc_aut(0) script static
void Boss_HInit(void) {
   ACS_Delay(1); /* Delay another tic for monster spawners. */

   SpawnBosses(pl.scoresum, false);
}

static
void CheckModCompat(void) {
   i32 tid;

   if((wl.legendoom = ACS_SpawnForced(so_LDLegendaryMonsterMarker, 0, 0, 0, tid = ACS_UniqueTID(), 0))) ACS_Thing_Remove(tid);

   /*drlamonsters = CVarGetI(sc_drla_is_using_monsters);*/
}

static
void UpdateGame(void) {
   i32 ver_num = version_name_to_num(tmpstr(CVarGetS(sc_version)));

   if(ver_num < vernum_1_5_2_0 && CVarGetI(sc_sv_difficulty) == 1) {
      CVarSetI(sc_sv_difficulty, 10);
   }

   if(ver_num < vernum_1_6_0_0 && CVarGetK(sc_player_footstepvol) == 1.0k) {
      CVarSetK(sc_player_footstepvol, 0.2k);
   }

   if(ver_num < vernum_1_6_1_0 && CVarGetK(sc_weapons_zoomfactor) == 3.0k) {
      CVarSetK(sc_weapons_zoomfactor, 1.5);
   }

   if(ver_num < vernum_1_6_3_0 && CVarGetI(sc_xhair_style) >= 10) {
      CVarSetI(sc_xhair_style, 0);
   }

   CVarSetS(sc_version, fast_strdup(verstr));
}

static
void MInitPre(void) {
   Dbg_Log(log_dev, _l(__func__));

   CheckModCompat();
   UpdateGame();
}

static
void GInit(void) {
   Dbg_Log(log_dev, _l(__func__));

   wl.cbiperf = 10;

   ml.islithmap = (MapNum >= LithMapBeg && MapNum <= LithMapEnd);

   Mon_Init();
   Wep_GInit();

   wl.gblinit = true;
}

static
void MInitPst(void) {
   Dbg_Log(log_dev, _l(__func__));

   wl.pay.par = ACS_GetLevelInfo(LEVELINFO_PAR_TIME) * 35;

   wl.pay.killmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_MONSTERS);
   wl.pay.itemmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_ITEMS);
   wl.pay.scrtmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_SECRETS);

   /* Check for if rain should be used.
    * - If `NoRain' is set on LithMapLine, never use rain.
    * - If the player has rain enabled, use it if not for those preconditions.
    * - If `UseRain' is set on LithMapLine, use it if not for that.
    */
   bool never_rain    = ACS_GetLineUDMFInt(LithMapLine, sm_MapNoRain);
   bool use_rain_user = CVarGetI(sc_sv_rain);
   bool use_rain_map  = ACS_GetLineUDMFInt(LithMapLine, sm_MapUseRain);
   if(!never_rain && (use_rain_user || use_rain_map)) {
      wl.dorain = true;
      W_DoRain();
   }

   ml.modinit = true;
}

static
void MInit(void) {
   Dbg_Log(log_dev, _l(__func__));

   Dlg_MInit();

   wl.soulsfreed = 0;

   /* Init a random seed from the map. */
   ml.mapseed = ACS_Random(0, INT32_MAX);

   /* Set the air control because ZDoom's default sucks. */
   ACS_SetAirControl(0.77);
}

static
void HInitPre(void) {
   Dbg_Log(log_dev, _l(__func__));

   if(wl.unloaded) {
      wl.mapscleared++;
   }

   wl.bossspawned = false;

   if(CVarGetI(sc_sv_sky) && !ml.islithmap) {
      if(MapNum >= 21) {
         ACS_ChangeSky(sp_LITHSKRD, sp_LITHSKRD);
         ACS_SetSkyScrollSpeed(1, 0.01);
      } else if(MapNum >= 12) {
         ACS_ChangeSky(sp_LITHSKDE, sp_LITHSKDE);
      } else {
         ACS_ChangeSky(sp_LITHSKS1, sp_LITHSKS1);
      }
   }
}

static
void HInit(void) {
   Dbg_Log(log_dev, _l(__func__));

   if(!CVarGetI(sc_sv_nobosses)) {
      Boss_HInit();
   }

   /* Payout, which is not done on the first map. */
   if(wl.mapscleared != 0) Scr_HInit();

   /* Cluster messages. */
   if(pl.pclass & pcl_outcasts) {
      if(wl.mapscleared == 10) P_BIP_Unlock(P_BIP_NameToPage("MCluster1"), false);
      if(wl.mapscleared == 20) P_BIP_Unlock(P_BIP_NameToPage("MCluster2"), false);
      if(wl.mapscleared == 25) P_BIP_Unlock(P_BIP_NameToPage("MCluster3"), false);
   }

   /* CBI */
   if(CVarGetI(sc_sv_nobosses) ||
      CVarGetI(sc_sv_nobossdrop) ||
      dbgflags(dbgf_items))
   {
      for(i32 i = 0; i < bossreward_max; i++) {
         CBI_Install(i);
      }
   }

   CBI_InstallSpawned();
}

dynam_aut script ext("ACS") addr(lsc_worldopen)
void Z_World(bool is_reopen) {
   Dbg_Log(log_dev, _l(__func__));

   Draw_Init();

   if(ACS_GameType() == GAME_TITLE_MAP) {
      W_Title();
      return;
   } else if(MapNum == 1911777) {
      pl.setActivator();
      ACS_SetPlayerProperty(true, true, PROP_TOTALLYFROZEN);
      Dlg_MInit();
      F_Run();
      return;
   }

   if(CVarGetI(sc_sv_failtime) == 0) for(;;) {
      ACS_BeginPrint();
      PrintChrLi(
         "\n=======\n"
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
         "(\"");
      ACS_PrintBind(sc_toggleconsole);
      PrintChrLi("\" or options menu) for more information.");
      ACS_Delay(10);
   }

   #ifndef NDEBUG
   dbgnotenum = 0;
   #endif

   /* Now, initialize everything.
    * Start by deallocating temporary tags.
    */
   Xalloc(_tag_temp);

   if(!ml.modinit) MInitPre();
   if(!wl.gblinit) GInit();
   if(!ml.modinit) MInit();

   /* Hub-static pre-player init. */
   if(!is_reopen) HInitPre();

   wl.unloaded = false; /* Unloaded flag can be reset now. */

   ACS_Delay(2); /* Wait for players to get initialized. */

   /* Hub-static post-player init. */
   if(!is_reopen) HInit();

   /* Module-static post-hub init. */
   if(!ml.modinit) MInitPst();

   /* Main loop. */
   i32 prevscrts = 0;
   i32 prevkills = 0;
   i32 previtems = 0;

   i32 missionkill = 0;
   i32 missionprc  = 0;

   for(;;) {
      if(wl.ticks > CVarGetI(sc_sv_failtime) * 35 * 60 * 60 && !ml.islithmap) {
         F_Start(_finale_time_out);
         return;
      }

      i32 scrts = ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS);
      i32 kills = ACS_GetLevelInfo(LEVELINFO_KILLED_MONSTERS);
      i32 items = ACS_GetLevelInfo(LEVELINFO_FOUND_ITEMS);

      if(kills > prevkills)  wl.pay.killnum  += kills - prevkills;
      if(items > previtems)  wl.pay.itemnum  += items - previtems;
      if(scrts > prevscrts) {wl.pay.scrtnum  += scrts - prevscrts;
                             wl.secretsfound += scrts - prevscrts;}

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

      Dbg_Stat(_l("mission%: "), _p(missionprc), _c('\n'));

      ACS_Delay(1);

      #ifndef NDEBUG
      dbgstatnum = 0;
      #endif
      wl.ticks++;

      i32 autosave = CVarGetI(sc_sv_autosave);
      if(autosave && wl.ticks % (35 * 60 * autosave) == 0) ACS_Autosave();
   }
}

script type("unloading") static
void Z_WorldUnload(void) {
   wl.unloaded = true;
   Dbg_Log(log_dev, _l(__func__));

   pl.setActivator();
   P_Upg_PDeinit();
   P_GUI_Close();
   P_Dat_PTickPst();
}

/* EOF */
