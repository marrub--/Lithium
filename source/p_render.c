/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Miscellaneous player effects.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "p_hudid.h"

/* Static Functions -------------------------------------------------------- */

static void P_Ren_Style(struct player *p) {
   if(p->scopetoken) {
      SetPropI(0, APROP_RenderStyle, STYLE_Subtract);
      SetPropK(0, APROP_Alpha, p->getCVarK(sc_weapons_scopealpha) * p->alpha);
   } else {
      SetPropI(0, APROP_RenderStyle, STYLE_Translucent);
      SetPropK(0, APROP_Alpha, p->getCVarK(sc_weapons_alpha) * p->alpha);
   }
}

static void P_Ren_Advice(struct player *p) {
   if(CheckFade(fid_advice)) {
      SetSize(640, 480);

      SetClipW(80, 100, 500, 500, 500);
      PrintTextF_str(p->advice, s_smallfnt, CR_YELLOW, 80,1, 100,1, fid_advice);
      ClearClip();
   }
}

static void P_Ren_LevelUp(struct player *p) {
   if(p->old.attr.level && p->old.attr.level < p->attr.level) {
      ACS_LocalAmbientSound(ss_player_levelup, 127);
      p->logH(1, LanguageC(LANG "LOG_LevelUp%s", p->discrim), ACS_Random(1000, 9000));
   }

   if(p->attr.lvupstr[0]) {
      SetSize(320, 240);
      PrintTextChr(p->attr.lvupstr, p->attr.lvupstrn);
      PrintText(s_smallfnt, CR_WHITE, 220,1, 75,1);
   }
}

/* Extern Functions -------------------------------------------------------- */

void P_Ren_PTickPst(struct player *p) {
   P_Ren_Magic(p);
   P_Ren_Step(p);
   P_Ren_View(p);
   P_Ren_Scope(p);
   P_Ren_Style(p);
   P_Ren_Advice(p);
   P_Ren_LevelUp(p);
   #ifndef NDEBUG
   P_Ren_Debug(p);
   #endif
}

alloc_aut(0) sync
void P_TeleportIn(struct player *p) {
   p->teleportedout = false;

   ACS_AmbientSound(ss_misc_telein, 127);
   ACS_SetCameraToTexture(p->tid, s_LITHCAM3, 90);

   for(i32 i = 18, j = 18; i >= 1; i--) {
      ACS_Delay(1);
      k32 w = 1 + (i / 18.0lk / 3.0lk * 50);
      k32 h = 1 + (j / 18.0lk / 8.0lk * 10);
      SetSize(320, 200);
      PrintSprite(sp_Terminal_Teleport, 160,0, 100,0);
      SetSize(640 / w, 480 * h);
      PrintSprite(s_LITHCAM3, 320/w,0, 240*h,0);
      if(i & 3) j--;
   }
}

alloc_aut(0) sync
void P_TeleportOut(struct player *p) {
   ACS_AmbientSound(ss_misc_teleout, 127);
   ACS_SetCameraToTexture(p->tid, s_LITHCAM3, 90);

   for(i32 i = 1, j = 1; i <= 18; i++) {
      k32 w = 1 + (i / 18.0lk / 3.0lk * 50);
      k32 h = 1 + (j / 18.0lk / 8.0lk * 10);
      SetSize(320, 200);
      PrintSprite(sp_Terminal_Teleport, 160,0, 100,0);
      SetSize(640 / w, 480 * h);
      PrintSprite(s_LITHCAM3, 320/w,0, 240*h,0);
      if(i & 3) j++;
      ACS_Delay(1);
   }

   p->teleportedout = true;
}

script void P_TeleportInAsync(struct player *p) {
   P_TeleportIn(p);
}

script void P_TeleportOutAsync(struct player *p) {
   P_TeleportOut(p);
}

/* EOF */
