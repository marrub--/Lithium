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
void P_Ren_Advice() {
   if(CheckFade(fid_advice)) {
      SetSize(640, 480);

      SetClipW(80, 100, 500, 500, 500);
      PrintTextF_str(pl.advice, sf_smallfnt, CR_YELLOW, 80,1, 100,1, fid_advice);
      ClearClip();
   }
}

static
void P_Ren_LevelUp() {
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

void P_Ren_PTickPst() {
   P_Ren_Magic();
   P_Ren_Step();
   P_Ren_View();
   P_Ren_Scope();
   P_Ren_Advice();
   P_Ren_LevelUp();
   #ifndef NDEBUG
   P_Ren_Debug();
   #endif
}

alloc_aut(0) script
void P_TeleportIn() {
   pl.teleportedout = false;

   ACS_AmbientSound(ss_misc_telein, 127);
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
   ACS_AmbientSound(ss_misc_teleout, 127);
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
