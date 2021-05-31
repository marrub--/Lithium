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

/* Static Objects ---------------------------------------------------------- */

Str(sp_lithcam3, s"LITHCAM3");

/* Static Functions -------------------------------------------------------- */

static void P_Ren_Advice() {
   if(CheckFade(fid_advice)) {
      SetSize(640, 480);

      SetClipW(80, 100, 500, 500, 500);
      PrintTextF_str(pl.advice, sf_smallfnt, CR_YELLOW, 80,1, 100,1, fid_advice);
      ClearClip();
   }
}

static void P_Ren_LevelUp() {
   if(pl.old.attr.level && pl.old.attr.level < pl.attr.level) {
      ACS_LocalAmbientSound(ss_player_levelup, 127);
      pl.logH(1, LanguageC(LANG "LOG_LevelUp%s", pl.discrim), ACS_Random(1000, 9000));
   }

   if(pl.attr.lvupstr[0]) {
      SetSize(320, 240);
      PrintTextChr(pl.attr.lvupstr, pl.attr.lvupstrn);
      PrintText(sf_smallfnt, CR_WHITE, 220,1, 75,1);
   }
}

/* Extern Functions -------------------------------------------------------- */

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

script_str ext("ACS") addr(OBJ "RenderCrosshair")
void P_Ren_Crosshair(int consolePlayer) {
   if(P_None()) return;

   pl.setActivator();

   Str(sc_xhair_r,            sCVAR "xhair_r");
   Str(sc_xhair_g,            sCVAR "xhair_g");
   Str(sc_xhair_b,            sCVAR "xhair_b");
   Str(sc_xhair_a,            sCVAR "xhair_a");
   Str(sc_xhair_style,        sCVAR "xhair_style");
   Str(sc_xhair_enable,       sCVAR "xhair_enable");
   Str(sc_xhair_enablejuicer, sCVAR "xhair_enablejuicer");

   SetSize(320, 240);

   /* trace to where the crosshair should be in world space */
   struct k32v3 loc = trace_from(pl.yaw   + pl.addyaw,
                                 pl.pitch + pl.addpitch,
                                 2048, GetAttackHeight());

   /* project */
   bool seen;
   struct i32v2 xh = project(loc.x, loc.y, loc.z, &seen);
   if(!seen) {
      return;
   }

   /* draw a tracer for the targeting system */
   if(P_Wep_CurType() == weapon_launcher &&
      ACS_SetActivator(0, AAPTR_TRACER) &&
      GetHealth(0) > 0) {
      k32 x = GetX(0);
      k32 y = GetY(0);
      k32 z = GetZ(0) + GetHeight(0) / 2;

      pl.setActivator();

      struct i32v2 th = project(x, y, z, &seen);

      if(seen) {
         PrintLine(xh.x, xh.y, th.x, th.y, 0xFF0000);
      }
   } else {
      pl.setActivator();
   }

   /* draw the crosshair */
   u32 r = pl.getCVarI(sc_xhair_r); if(r > 255) r = 255;
   u32 g = pl.getCVarI(sc_xhair_g); if(g > 255) g = 255;
   u32 b = pl.getCVarI(sc_xhair_b); if(b > 255) b = 255;
   k32 a = pl.getCVarI(sc_xhair_a); if(a > 255) a = 255; a /= 255.0k;

   if(pl.getCVarI(sc_xhair_enable) && a > 0.0k) {
      StrAry(xb, s":XHairs:circleb", s":XHairs:deltab",  s":XHairs:ovalb",
                 s":XHairs:basicb",  s":XHairs:delearb", s":XHairs:finib",
                 s":XHairs:angleb",  s":XHairs:dotb",    s":XHairs:xpb");
      StrAry(xw, s":XHairs:circlew", s":XHairs:deltaw",  s":XHairs:ovalw",
                 s":XHairs:basicw",  s":XHairs:delearw", s":XHairs:finiw",
                 s":XHairs:anglew",  s":XHairs:dotw",    s":XHairs:xpw");

      u32 c     = (r << 16) | (g << 8) | (b << 0);
      i32 style = pl.getCVarI(sc_xhair_style);
      PrintSpriteA (xb[style], xh.x,0, xh.y,0, a);
      PrintSpriteAC(xw[style], xh.x,0, xh.y,0, a, c);

      if(pl.getCVarI(sc_xhair_enablejuicer)) {
         Str(sp_xhairs_l, s":XHairs:L");
         Str(sp_xhairs_r, s":XHairs:R");

         i32 xp = ceilk(pl.extrpitch * 500.0k) + 10;
         PrintSpriteAC(sp_xhairs_l, xh.x - xp,0, xh.y,0, a, c);
         PrintSpriteAC(sp_xhairs_r, xh.x + xp,0, xh.y,0, a, c);
      }
   }
}

alloc_aut(0) script
void P_TeleportIn() {
   pl.teleportedout = false;

   ACS_AmbientSound(ss_misc_telein, 127);
   ACS_SetCameraToTexture(pl.tid, sp_lithcam3, 90);

   for(i32 i = 18, j = 18; i >= 1; i--) {
      ACS_Delay(1);
      k32 w = 1 + (i / 18.0lk / 3.0lk * 50);
      k32 h = 1 + (j / 18.0lk / 8.0lk * 10);
      SetSize(320, 200);
      PrintSprite(sp_Terminal_Teleport, 160,0, 100,0);
      SetSize(640 / w, 480 * h);
      PrintSprite(sp_lithcam3, 320/w,0, 240*h,0);
      if(i & 3) j--;
   }
}

alloc_aut(0) sync
void P_TeleportOut(i32 tag) {
   ACS_AmbientSound(ss_misc_teleout, 127);
   ACS_SetCameraToTexture(pl.tid, sp_lithcam3, 90);

   for(i32 i = 1, j = 1; i <= 20; i++) {
      ACS_Delay(1);
      k32 w = 1 + i / 20.0lk / 3.0lk * 50;
      k32 h = 1 + j / 20.0lk / 8.0lk * 10;
      SetSize(320, 200);
      PrintSprite(sp_Terminal_Teleport, 160,0, 100,0);
      SetSize(640 / w, 480 * h);
      PrintSprite(sp_lithcam3, 320/w,0, 240*h,0);
      if(i & 3) j++;
   }

   pl.teleportedout = true;

   ACS_Teleport_NewMap(tag, 0, false);
}

/* EOF */
