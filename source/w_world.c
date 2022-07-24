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
noinit struct cvars            cv;

script void SpawnBosses(score_t sum, bool force);

i32 UniqueID(i32 tid) {
   i32 pn;

   /* Negative values are for players. */
   if((pn = PtrPlayerNumber(tid)) != -1) return -(pn + 1);

   /* If we already have a unique identifier, return that. */
   i32 id = PtrInvNum(tid, so_UniqueID);

   /* Otherwise we have to give a new unique identifier. */
   if(id == 0) PtrInvGive(tid, so_UniqueID, id = ++ml.id);

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

   if(EDataI(_edt_legendoom))              set_bit(wl.compat, _comp_legendoom);
   if(EDataI(_edt_rampancy))               set_bit(wl.compat, _comp_rampancy);
   if(CVarGetI(sc_drla_is_using_monsters)) set_bit(wl.compat, _comp_drla);
   if(EDataI(_edt_colorfulhell))           set_bit(wl.compat, _comp_ch);
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

   CVarSetS(sc_version, st_ver);
}

static
void MInitPre(void) {
   Dbg_Log(log_dev, _l(__func__));
   #define cvar_map(ty, na) cv.na = cvar_get(na);
   #define cvar_x(ev, na, ty) cvar_##ev(ty, na)
   #include "common.h"
   /* Init a random seed from the map. */
   ml.seed = ACS_Random(0, INT32_MAX);
   /* Set the air control because ZDoom's default sucks. */
   ACS_SetAirControl(0.77);
   /* Set up everything else about the map. -T */
   ml.soulsfreed = 0;
   srand(ml.seed);
   ml.lump = strp(ACS_PrintName(PRINTNAME_LEVEL));
   ml.name = strp(ACS_PrintName(PRINTNAME_LEVELNAME));
   ml.boss = EDataI(_edt_bosslevel);
   if(MapNum >= LithMapBeg && MapNum <= LithMapEnd) {
      set_msk(ml.flag, _mapf_cat, _mapc_lithium);
   }
   if(ml.boss == boss_iconofsin && GetFun() & lfun_tainted) {
      set_bit(ml.flag, _mapf_corrupted);
   }
   /* TODO: check for map's built-in dewpoint and temp */
   i32 dewpoint    = rand() % 11 - 1;
   ml.temperature  = rand() % 100;
   ml.humidity     = ml.temperature + dewpoint;
   ml.humidity     = mini(ml.humidity * ml.humidity / 90, 100);
   ml.temperature -= 30;
   /* TODO: check for map's built-in thunder and snow settings */
   i32 thunder_chk = rand() % 99;
   i32    snow_chk = rand() % 99;
   if(ml.humidity > 0) {
      if(ml.temperature >= 12 + dewpoint && thunder_chk < 33) {
         set_bit(ml.flag, _mapf_thunder);
      }
      switch(CVarGetI(sc_sv_rain)) {
      case 1:
         if(ml.humidity > 60 + dewpoint) {
         case 2:
            set_msk(ml.flag, _mapf_rain, _mapr_rain);
         } else if(ml.temperature <= 0 && snow_chk < 11) {
         case 3:
            set_msk(ml.flag, _mapf_rain, _mapr_snow);
         }
      }
   } else {
      set_bit(ml.flag, _mapf_vacuum);
      ml.humidity    = 0;
      ml.temperature = -270;
   }
   /* TODO: check for map's sky setting instead */
   if(CVarGetI(sc_sv_sky) && get_msk(ml.flag, _mapf_cat) != _mapc_lithium) {
      set_bit(ml.flag, _mapf_skyreplace);
   }
   str sky = fast_strupper(EDataS(_edt_sky1));
   if(sky == sp_SKY2 || sky == sp_RSKY2) {
      set_msk(ml.flag, _mapf_cat, _mapc_interstice);
   } else if(sky == sp_SKY3 || sky == sp_SKY4 || sky == sp_RSKY3) {
      set_msk(ml.flag, _mapf_cat, _mapc_hell);
   }
   W_DoRain();
}

static
void GInitPre(void) {
   Dbg_Log(log_dev, _l(__func__));
   CheckModCompat();
   UpdateGame();
   #define cvar_gbl(ty, na) cv.na = cvar_get(na);
   #define cvar_x(ev, na, ty) cvar_##ev(ty, na)
   #include "common.h"
}

static
void GInit(void) {
   Dbg_Log(log_dev, _l(__func__));

   wl.cbiperf = 10;

   Mon_Init();
   Wep_GInit();

   wl.init = true;
}

static
void MInitPst(void) {
   Dbg_Log(log_dev, _l(__func__));

   wl.pay.par = ACS_GetLevelInfo(LEVELINFO_PAR_TIME) * 35;

   wl.pay.killmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_MONSTERS);
   wl.pay.itemmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_ITEMS);
   wl.pay.scrtmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_SECRETS);

   ml.init = true;
}

static
void MInit(void) {
   Dbg_Log(log_dev, _l(__func__));

   Dlg_MInit();
}

static
void HInitPre(void) {
   Dbg_Log(log_dev, _l(__func__));

   if(wl.unloaded) {
      wl.mapscleared++;
   }

   wl.bossspawned = false;

   if(get_bit(ml.flag, _mapf_skyreplace)) {
      if(get_msk(ml.flag, _mapf_cat) == _mapc_hell) {
         ACS_ChangeSky(sp_LITHSKRD, sp_LITHSKRD);
         ACS_SetSkyScrollSpeed(1, 0.01);
      } else if(get_msk(ml.flag, _mapf_cat) == _mapc_interstice) {
         ACS_ChangeSky(sp_LITHSKDE, sp_LITHSKDE);
      } else {
         ACS_ChangeSky(sp_LITHSKS1, sp_LITHSKS1);
      }
   }
}

static
void HInit(void) {
   Dbg_Log(log_dev, _l(__func__));

   if(!cv.sv_nobosses) {
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
}

dynam_aut script ext("ACS") addr(lsc_worldopen)
void Z_World(bool is_reopen) {
   Dbg_Log(log_dev, _l(__func__));

   if(ACS_GameType() == GAME_TITLE_MAP) {
      set_msk(ml.flag, _mapf_kind, _mapk_title);
   } else if(MapNum == LithMapEnd) {
      set_msk(ml.flag, _mapf_kind, _mapk_end);
   } else {
      set_msk(ml.flag, _mapf_kind, _mapk_normal);
   }

   Draw_Init();

   if(CVarGetI(sc_sv_failtime) == 0) for(;;) {
      SetSize(320, 240);
      SetClipW(0, 0, 320, 240, 320);
      PrintTextChL(
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
         "try these steps again or delete your GZDoom configuration.");
      PrintText(sf_ltrmfont, CR_WHITE, 0,1, 16,1);
      ClearClip();
      ACS_Delay(1);
   }

   #ifndef NDEBUG
   dbgnotenum = 0;
   #endif

   /* Now, initialize everything.
    * Start by deallocating temporary tags.
    */
   Xalloc(_tag_temp);

   if(!wl.init) GInitPre();
   if(!ml.init) MInitPre();
   if(!wl.init) GInit();
   if(!ml.init) MInit();

   /* Hub-static pre-player init. */
   if(!is_reopen) HInitPre();

   wl.unloaded = false; /* Unloaded flag can be reset now. */

   /* Special map main-loop functions. */
   switch(get_msk(ml.flag, _mapf_kind)) {
   case _mapk_title:
      W_Title();
      return;
   case _mapk_end:
      F_Run();
      return;
   }

   ACS_Delay(2); /* Wait for players to get initialized. */

   /* Hub-static post-player init. */
   if(!is_reopen) HInit();

   /* Module-static post-hub init. */
   if(!ml.init) MInitPst();

   /* Main loop. */
   i32 prevscrts = 0;
   i32 prevkills = 0;
   i32 previtems = 0;

   i32 missionkill = 0;
   i32 missionprc  = 0;

   for(;;) {
      if(wl.ticks > cv.sv_failtime * 35 * 60 * 60 && get_msk(ml.flag, _mapf_cat) != _mapc_lithium) {
         F_Start(_finale_time_out);
         return;
      }

      #define cvar_tic(ty, na) cv.na = cvar_get(na);
      #define cvar_x(ev, na, ty) cvar_##ev(ty, na)
      #include "common.h"

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

      if(cv.sv_autosave && wl.ticks % (35 * 60 * cv.sv_autosave) == 0) {
         ACS_Autosave();
      }
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
