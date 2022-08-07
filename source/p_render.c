// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Miscellaneous player effects.                                            │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "p_hudid.h"

static
void P_Ren_LevelUp(void) {
   if(pl.old.attr.level && pl.old.attr.level < pl.attr.level) {
      AmbientSound(ss_player_levelup, 1.0);
      pl.logH(1, tmpstr(lang_discrim(sl_log_levelup)), ACS_Random(1000, 9000));
   }

   if(pl.attr.lvupstr[0]) {
      SetSize(320, 240);
      PrintTextChr(pl.attr.lvupstr, pl.attr.lvupstrn);
      PrintText(sf_smallfnt, CR_WHITE, 220,1, 75,1);
   }
}

script_str ext("ACS") addr(OBJ "ShowMission") void Z_ShowMission(void) {
   pl.missionstatshow = ACS_Timer() + 105;
}

alloc_aut(0) stkcall static
void P_Ren_Mission(void) {
   i32 delta = ACS_Timer() - pl.missionstatshow;
   if(delta < 0) {
      k32 alpha = delta > -43 ? ease_out_cubic(delta / -43.0k) : 1.0k;
      SetSize(320, 240);
      ACS_BeginPrint();
      _p(fast_strupper(ml.name)); _c('\n');
      _l("\Cj"); _p(CanonTime(ct_full, ACS_Timer())); _c('\n');
      if(get_bit(ml.flag, _mflg_vacuum)) {
         _l("\CiVACUUM\n");
      } else {
         _l("\Cv"); _p(ml.temperature); _l(u8"°C\n");
         _l("\Cy"); _p(ml.humidity);    _l("%RH\n");
      }
      switch(get_msk(ml.flag, _mflg_rain)) {
      case _rain_rain:  _l("\CnRAINING (WATER)\n");   break;
      case _rain_blood: _l("\CgRAINING (UNKNOWN)\n"); break;
      case _rain_snow:  _l("\CwSNOWING\n");           break;
      case _rain_fire:  _l("\CxFIRESTORM\n");         break;
      }
      if(get_bit(ml.flag, _mflg_thunder))   {_l("\CkELEC. STORM\n");}
      if(get_bit(ml.flag, _mflg_corrupted)) {_l("\CgCAUSALITY SHIFT\n");}
      switch(get_msk(ml.flag, _mflg_env)) {
      case _menv_none:       _l("\CjAREA \CnNOMINAL\n");        break;
      case _menv_interstice: _l("\CjAREA \CiIRREGULAR\n");      break;
      case _menv_hell:       _l("\CjAREA \CgNOT IN REALITY\n"); break;
      case _menv_abyss:      _l("\CjAREA \CmUNKNOWN\n");        break;
      case _menv_evil:       _l("\CjAREA \CrCORRUPTED\n");      break;
      }
      switch(ml.mission) {
      case _mstat_unfinished: _l("\CjMISSION \CiUNFINISHED"); break;
      case _mstat_finished:   _l("\CjMISSION \CdFINISHED");   break;
      case _mstat_failure:    _l("\CjMISSION \CgFAILED");     break;
      }
      PrintTextA(sf_smallfnt, pl.color, 320,3, 28,1, alpha);
   }
}

static void P_Ren_Magic(void) {
   SetSize(800, 600);

   for(i32 i = 0; i < 4; i++) {
      i32 fid = fid_rendS + i;
      if(CheckFade(fid)) {
         PrintSpriteFP(pl.rendhit ? sa_rend_ht[i] : sa_rend_nh[i],
                       400,0, 300,0, fid);
      }
   }

   if(CheckFade(fid_blade)) {
      PrintSpriteFP(pl.bladehit ? sp_Weapon_BladeHit : sp_Weapon_Blade,
                    400,0, 300,0, fid_blade);
   }
}

void P_Ren_PTickPst(void) {
   P_Ren_Magic();
   P_Ren_Step();
   P_Ren_View();
   P_Ren_Scope();
   P_Ren_LevelUp();
   #ifndef NDEBUG
   P_Ren_Debug();
   #endif
   P_Ren_Mission();
}

alloc_aut(0) script
void P_TeleportIn(void) {
   pl.teleportedout = false;

   AmbientSound(ss_misc_telein, 0.5k);
   ACS_SetCameraToTexture(pl.tid, sp_LITHCAM3, 90);

   for(i32 i = 18, j = 18; i >= 1; i--) {
      ACS_Delay(1);
      k32 w = 1 + (i / 18.0lk / 3.0lk * 50);
      k32 h = 1 + (j / 18.0lk / 8.0lk * 10);
      SetSize(320, 200);
      PrintSprite(sp_Terminal_Teleport, 160,0, 100,0);
      SetSize(640 / w, 480 * h);
      PrintSprite(sp_LITHCAM3, 320/w,0, 240*h,0);
      if(i & 3) j--;
   }
}

alloc_aut(0) sync
void P_TeleportOut(i32 tag) {
   AmbientSound(ss_misc_teleout, 0.5k);
   ACS_SetCameraToTexture(pl.tid, sp_LITHCAM3, 90);

   for(i32 i = 1, j = 1; i <= 20; i++) {
      ACS_Delay(1);
      k32 w = 1 + i / 20.0lk / 3.0lk * 50;
      k32 h = 1 + j / 20.0lk / 8.0lk * 10;
      SetSize(320, 200);
      PrintSprite(sp_Terminal_Teleport, 160,0, 100,0);
      SetSize(640 / w, 480 * h);
      PrintSprite(sp_LITHCAM3, 320/w,0, 240*h,0);
      if(i & 3) j++;
   }

   pl.teleportedout = true;

   ACS_Teleport_NewMap(tag, 0, false);
}

/* EOF */
