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

alloc_aut(0) stkcall static
void P_Ren_Mission(void) {
   if(pl.missionstatshow) {
      k32 ss    = pl.missionstatshow--;
      k32 max   = pl.missionstatshowmax;
      k32 quart = max / 4.0k;
      k32 alpha = ss < quart ? ease_out_cubic(ss / quart) : 1.0k;
      SetSize(320, 240);
      ACS_BeginPrint();
      _p(fast_strupper(ml.name)); _c('\n');
      _l("\Cj"); _p(CanonTime(ct_full, wl.ticks)); _c('\n');
      if(get_bit(ml.flag, _mapf_vacuum)) {
         _l("\CjENV: \CiVACUUM\n");
      } else {
         _l("\CjTEMPERATURE: \Cv"); _p(ml.temperature); _l(u8"°C\n");
         _l("\CjHUMIDITY: \Cy");    _p(ml.humidity);    _l("%\n");
      }
      switch(get_msk(ml.flag, _mapf_rain)) {
      case _mapr_rain:  _l("\CjENV: \CnRAINING\n");       break;
      case _mapr_blood: _l("\CjENV: \CgSANGUINE RAIN\n"); break;
      case _mapr_snow:  _l("\CjENV: \CwSNOWING\n");       break;
      }
      if(get_bit(ml.flag, _mapf_thunder))   _l("\CjENV: \CkELEC. STORM\n");
      if(get_bit(ml.flag, _mapf_corrupted)) _l("\CjENV: \CgCAUSALITY SHIFT\n");
      if(get_msk(ml.flag, _mapf_cat) == _mapc_lithium) {
         switch(ml.mission) {
         case _mstat_unfinished: _l("\CjMISSION: \CiUNFINISHED"); break;
         case _mstat_finished:   _l("\CjMISSION: \CdFINISHED");   break;
         case _mstat_failure:    _l("\CjMISSION: \CgFAILURE");    break;
         }
      } else {
         _l("\CjMISSION: \Ci");
         if(!ml.boss) {
            switch(pl.pclass) {
            case pcl_marine:    _l("UNFINISHED");    break;
            case pcl_cybermage: _l("ESCAPE");        break;
            case pcl_informant: _l("BECOME SAVIOR"); break;
            case pcl_wanderer:  _l("ROAM");          break;
            case pcl_assassin:  _l("GET RICH");      break;
            case pcl_darklord:  _l("FIND OMI");      break;
            case pcl_thoth:     _l("FIX THIS");      break;
            }
         } else {
            switch(pl.pclass) {
            case pcl_marine:    _l("SURVIVE");            break;
            case pcl_cybermage: _l("ESCAPE");             break;
            case pcl_informant: _l("ACTUALIZE");          break;
            case pcl_wanderer:  _l("BECOME MORE");        break;
            case pcl_assassin:  _l("SAVE MYSELF");        break;
            case pcl_darklord:  _l("FIND OURSELF");       break;
            case pcl_thoth:     _l("REASSERT CAUSALITY"); break;
            }
         }
      }
      PrintTextA(sf_smallfnt, pl.color, 320,3, 28,1, alpha);
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
