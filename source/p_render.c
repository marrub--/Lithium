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

#include "m_engine.h"
#include "p_player.h"
#include "p_hudid.h"

static void P_Ren_LevelUp(void) {
   if(pl.old.attr.level && pl.old.attr.level < pl.attr.level) {
      AmbientSound(ss_player_levelup, 1.0);
      P_LogH(1, tmpstr(lang_discrim(sl_log_levelup)), ACS_Random(1000, 9000));
   }

   if(pl.attr.lvupstr[0]) {
      SetSize(320, 240);
      BeginPrintStrN(pl.attr.lvupstr, pl.attr.lvupstrn);
      PrintText(sf_smallfnt, CR_WHITE, pl.hudrpos-100,1, 75,1);
   }
}

stkoff static void P_Ren_Mission(void) {
   if(ACS_Timer() == 0 || !pl.hudenabled) {
      return;
   }
   if(pl.missionstatshow >= 0) {
      k32 alpha = pl.missionstatshow <= 35 ?
         ease_out_sine(pl.missionstatshow / 35.0k) :
         1.0k;
      SetSize(320, 240);
      ACS_BeginPrint();
      _p(fast_strupper(ml.name)); _c('\n');
      _l("\Cj"); _p(CanonTime(ct_full, ACS_Timer())); _c('\n');
      if(get_bit(ml.flag, _mflg_vacuum)) {
         _l("\CiVACUUM\n");
      } else {
         static const cstr angles[] = {
            u8"↑", u8"↖", u8"←", u8"↙",
            u8"↓", u8"↘", u8"→", u8"↗",
         };
         _l("\Cv");  _p(ml.temperature); _l(u8"°C\n");
         _l("\Cy");  _p(ml.humidity);    _l("%RH\n");
         _l("\Ce");  _p(EDataI(_edt_windspeed));
         i32 ang = EDataI(_edt_windangle) - (i32)(pl.yaw * 360.0k);
         _l("m/s "); _p(angles[const_deg(ang) / 45]);
         _l("\n");
      }
      switch(get_msk(ml.flag, _mflg_rain)) {
      case _rain_rain:  _l("\CnRAINING (WATER)\n");   break;
      case _rain_blood: _l("\CgRAINING (UNKNOWN)\n"); break;
      case _rain_snow:  _l("\CwRAINING (SNOW)\n");    break;
      case _rain_fire:  _l("\CxRAINING (ASH)\n");     break;
      }
      if(get_bit(ml.flag, _mflg_lightning)) {_l("\CkELEC. STORM\n");}
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
      PrintText(sf_smallfnt, pl.color, pl.hudrpos,3, 28,1, _u_no_unicode|_u_alpha, alpha);
      if(!Paused) {
         --pl.missionstatshow;
      }
   }
}

static void P_Ren_Magic(void) {
   SetSize(800, 600);

   for(i32 i = 0; i < 4; i++) {
      i32 fid = fid_rendS + i;
      if(CheckFade(fid)) {
         PrintSprite(pl.rendhit ? sa_rend_ht[i] : sa_rend_nh[i], 400,0, 300,0, _u_add|_u_fade, fid);
      }
   }

   if(CheckFade(fid_blade)) {
      PrintSprite(pl.bladehit ? sp_Weapon_BladeHit : sp_Weapon_Blade, 400,0, 300,0, _u_add|_u_fade, fid_blade);
   }
}

void P_Ren_PTick(void) {
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

alloc_aut(0) script void P_TeleportIn(void) {
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

alloc_aut(0) sync void P_TeleportOut(i32 tag) {
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
