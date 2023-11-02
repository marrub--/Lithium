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

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"
#include "w_monster.h"
#include "m_version.h"
#include "m_trie.h"

__addrdef __mod_arr lmvar;

noinit struct world            wl;
noinit struct map_locals lmvar ml;
noinit struct map_locals       oldml;
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

static void CheckModCompat(void) {
   i32 tid;

   if(EDataI(_edt_legendoom))              set_bit(wl.compat, _comp_legendoom);
   if(EDataI(_edt_rampancy))               set_bit(wl.compat, _comp_rampancy);
   if(CVarGetI(sc_drla_is_using_monsters)) set_bit(wl.compat, _comp_drla);
   if(EDataI(_edt_colorfulhell))           set_bit(wl.compat, _comp_ch);
}

static void UpdateGame(void) {
   i32 ver_num = version_name_to_num(tmpstr(CVarGetS(sc_version)));

   if(ver_num < vernum_1_5_2_0 && CVarGetI(sc_sv_difficulty) == 1) {
      CVarSetI(sc_sv_difficulty, 10);
   }

   if(ver_num < vernum_1_6_1_0 && CVarGetK(sc_weapons_zoomfactor) == 3.0k) {
      CVarSetK(sc_weapons_zoomfactor, 1.5);
   }

   CVarSetS(sc_version, st_ver);
}

script static void GInit(void) {
   Dbg_Log(log_dev, _l(_f));
   CheckModCompat();
   UpdateGame();
   wl.scorethreshold = _scorethreshold_default;
   wl.cbiperf = 10;
   wl.realtime = 51310938653; /* 14:30:53 26-7-1649 */
   Mon_Init();
   wl.init = true;
}

static void MInitGetStaticInfo(void) {
   struct map_info *mi = ml.mi = GetMapInfo();
   mi_setup(ml.mi);
   i32 fun = GetFun();
   ml.seed = mi_opt(ml.mi, _mi_key_seed, _v.i, ACS_Random(0, INT32_MAX));
   srand(ml.seed);
   ml.boss = EDataI(_edt_bosslevel);
   i32 func = _mfunc_normal;
   if(ACS_GameType() == GAME_TITLE_MAP) {
      func = _mfunc_title;
   } else if(ml.lump == sp_LITHEND) {
      func = _mfunc_end;
   }
   if(mi_flg(ml.mi, _mi_flag_nophantom) || cv.sv_nobosses) {
      set_msk(ml.flag, _mflg_boss, _mphantom_nospawn);
   }
   if(ml.boss == boss_iconofsin && fun & lfun_tainted) {
      set_bit(ml.flag, _mflg_corrupted);
   }
   i32 env = _menv_none;
   str sky = fast_strupper(EDataS(_edt_origsky1));
   if(get_bit(ml.mi->use, _mi_key_environment)) {
      env = ml.mi->keys[_mi_key_environment].i;
   } else if(fun & lfun_ragnarok) {
      env = _menv_evil;
   } else if(sky == sp_SKY2 || sky == sp_RSKY2) {
      env = _menv_interstice;
   } else if(sky == sp_SKY3 || sky == sp_SKY4 || sky == sp_RSKY3) {
      env = _menv_hell;
   }
   ml.humidity    = mi_opt(ml.mi, _mi_key_humidity,    _v.i, rand() % 101);
   ml.temperature = mi_opt(ml.mi, _mi_key_temperature, _v.i, rand() % 301 - 100);
   ml.windspeed   = mi_opt(ml.mi, _mi_key_windspeed,   _v.i, rand() % 100);
   i32  lrnd      = ml.temperature * 12 / 55;
   i32  hrnd      = fastabs(ml.temperature / 2) * (ml.humidity / 40);
   bool lightning = rand() % 99 < lrnd && !mi_flg(ml.mi, _mi_flag_nolightning);
   bool any_rain  = rand() % 99 < hrnd && !mi_flg(ml.mi, _mi_flag_norain);
   i32  rain      = _rain_none;
   if(ml.temperature > -90) {
      switch(CVarGetI(sc_sv_rain)) {
      case 1:
         if(any_rain) {
         case 2:
            switch(env) {
            case _menv_hell:
               if(ml.temperature >= 100) {rain = _rain_fire;}
               else                      {rain = _rain_blood;}
               break;
            default:
               if(ml.temperature <= 0) {rain = _rain_snow;}
               else                    {rain = _rain_rain;}
               break;
            }
         }
      }
   } else {
      set_bit(ml.flag, _mflg_vacuum);
      ml.humidity    = 0;
      ml.temperature = -270;
      if(env == _menv_abyss) {
         rain = _rain_abyss;
      }
   }
   if(EDataI(_edt_lightning) || lightning) {
      set_bit(ml.flag, _mflg_lightning);
   }
   set_msk(ml.flag, _mflg_rain, rain);
   set_msk(ml.flag, _mflg_func, func);
   set_msk(ml.flag, _mflg_env,  env);
   set_msk(ml.flag, _mflg_sky,  mi_opt(mi, _mi_key_sky, _v.i, CVarGetI(sc_sv_sky)));
}

script static void MInit(void) {
   Dbg_Log(log_dev, _l(_f));
   ml.lump = strp(ACS_PrintName(PRINTNAME_LEVEL));
   ml.name = strp(ACS_PrintName(PRINTNAME_LEVELNAME));
   DefaultAirControl();
   ml.soulsfreed = 0;
   MInitGetStaticInfo();
   ServCallV(sm_UpdateSky);
   Dlg_MInit();
   SpawnBosses(pl.scoresum, false);
}

static void MInitPst(void) {
   Dbg_Log(log_dev, _l(_f));

   if(wl.hubscleared != 0) {
      Scr_MInit();
   }

   if(get_bit(pcl_outcasts, pl.pclass)) {
      if(wl.hubscleared == 10) P_BIP_Unlock(P_BIP_NameToPage("MCluster1"), false);
      if(wl.hubscleared == 20) P_BIP_Unlock(P_BIP_NameToPage("MCluster2"), false);
      if(wl.hubscleared == 25) P_BIP_Unlock(P_BIP_NameToPage("MCluster3"), false);
   }

   /* TODO: figure out what to do with these in a hub setup */
   wl.pay.par = ACS_GetLevelInfo(LEVELINFO_PAR_TIME) * 35;

   wl.pay.killmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_MONSTERS);
   wl.pay.itemmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_ITEMS);
   wl.pay.scrtmax += ACS_GetLevelInfo(LEVELINFO_TOTAL_SECRETS);
}

dynam_aut script void W_World(void) {
   Dbg_Log(log_dev, _l(_f));
   MInitPst();
   P_Player();
   /* Main loop. */
   for(;;) {
      if(ACS_Timer() > cv.sv_failtime * 35 * 60 * 60) {
         F_Start(_finale_time_out);
         return;
      }
      #define cvar_tic(ty, na) cv.na = cvar_get(na);
      #define cvar_x(ev, na, ty) cvar_##ev(ty, na)
      #include "m_engine.h"
      i32 scrts = ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS);
      i32 kills = ACS_GetLevelInfo(LEVELINFO_KILLED_MONSTERS);
      i32 items = ACS_GetLevelInfo(LEVELINFO_FOUND_ITEMS);
      if(kills > ml.prevkills)  wl.pay.killnum  += kills - ml.prevkills;
      if(items > ml.previtems)  wl.pay.itemnum  += items - ml.previtems;
      if(scrts > ml.prevscrts) {wl.pay.scrtnum  += scrts - ml.prevscrts;
                                wl.secretsfound += scrts - ml.prevscrts;}
      ml.prevscrts = scrts;
      ml.prevkills = kills;
      ml.previtems = items;
      if(ACS_Timer() % 5 == 0 && ml.missionkill < kills) {
         if(++ml.missionprc >= 150) {
            SpawnBosses(0, true);
            ml.missionprc = 0;
         }
         ml.missionkill = kills;
      }
      ACS_Delay(1);
      if(cv.sv_autosave && ACS_Timer() && ACS_Timer() % (35 * 60 * cv.sv_autosave) == 0) {
         ACS_Autosave();
      }
      W_TickTime();
   }
}

alloc_aut(0) script static void W_WrongConfig() {
   Dbg_Log(log_dev, _l(_f));
   for(;;) {
      SetSize(320, 240);
      BeginPrintStrL(
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
      PrintText(sf_ltrmfont, CR_WHITE, 0,1, 16,1, 0, 0, 320);
      ACS_Delay(1);
   }
}

script ext("ACS") addr(lsc_gsinit) void GsInit(void);

alloc_aut(0) script ext("ACS") addr(lsc_worldopen) void Z_World(void) {
   P_EarlyInit();
   if(!wl.init) {
      GsInit();
      StrInit();
      DrawInit();
      #define cvar_gbl(ty, na) cv.na = cvar_get(na);
      #define cvar_x(ev, na, ty) cvar_##ev(ty, na)
      #include "m_engine.h"
   }

   /* TODO: handle map cvars properly in hubs */
   #define cvar_map(ty, na) cv.na = cvar_get(na);
   #define cvar_tic(ty, na) cv.na = cvar_get(na);
   #define cvar_x(ev, na, ty) cvar_##ev(ty, na)
   #include "m_engine.h"

   Dbg_Log(log_dev, _l(_f));

   if(CVarGetI(sc_sv_failtime) == 0) {
      W_WrongConfig();
      return;
   }

   if(!wl.init) {
      GInit();
   }
   MInit();

   /* choose main-loop function */
   switch(get_msk(ml.flag, _mflg_func)) {
   case _mfunc_title: W_Title(); break;
   case _mfunc_end:   F_Run();   break;
   default:           W_World(); break;
   }
}

script type("unloading") static void Z_WorldUnload(void) {
   Dbg_Log(log_dev, _l(_f));
   oldml = ml;
   pl.setActivator();
   P_GUI_Close();
   P_Dat_PTick();
}

script ext("ACS") addr(lsc_hubclear) void Z_HubCleared(void) {
   Dbg_Log(log_dev, _l(_f));
   wl.hubscleared++;
   pl.setActivator();
}

/* EOF */
