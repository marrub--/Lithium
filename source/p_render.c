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

static void P_Ren_Style(struct player *p) {
   if(p->scopetoken) {
      SetRenderStyle(0, STYLE_Subtract);
      SetAlpha(0, p->getCVarK(sc_weapons_scopealpha) * p->alpha);
   } else {
      SetRenderStyle(0, STYLE_Translucent);
      SetAlpha(0, p->getCVarK(sc_weapons_alpha) * p->alpha);
   }
}

static void P_Ren_Advice(struct player *p) {
   if(CheckFade(fid_advice)) {
      SetSize(640, 480);

      SetClipW(80, 100, 500, 500, 500);
      PrintTextF_str(p->advice, sf_smallfnt, CR_YELLOW, 80,1, 100,1, fid_advice);
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
      PrintText(sf_smallfnt, CR_WHITE, 220,1, 75,1);
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

void P_Ren_Crosshair(struct player *p) {
   Str(sc_xhair_r,            sCVAR "xhair_r");
   Str(sc_xhair_g,            sCVAR "xhair_g");
   Str(sc_xhair_b,            sCVAR "xhair_b");
   Str(sc_xhair_a,            sCVAR "xhair_a");
   Str(sc_xhair_style,        sCVAR "xhair_style");
   Str(sc_xhair_enable,       sCVAR "xhair_enable");
   Str(sc_xhair_enablejuicer, sCVAR "xhair_enablejuicer");

   SetSize(320, 240);

   /* trace to where the crosshair should be in world space */
   struct k32v3 loc = trace_from(p->yaw   + p->addyaw,
                                 p->pitch + p->addpitch,
                                 2048, p->attackheight);

   /* unproject */
   bool seen;
   struct i32v2 xh = unproject(loc.x, loc.y, loc.z, &seen);
   if(!seen) {
      return;
   }

   /* draw a tracer for the targeting system */
   if(P_Wep_CurType(p) == weapon_launcher &&
      ACS_SetActivator(0, AAPTR_TRACER) &&
      GetHealth(0) > 0) {
      k32 x = GetX(0);
      k32 y = GetY(0);
      k32 z = GetZ(0) + GetHeight(0) / 2;

      struct i32v2 th = unproject(x, y, z, &seen);

      if(seen) {
         PrintLine(xh.x, xh.y, th.x, th.y, 0xFF0000);
      }
   }

   p->setActivator();

   /* draw the crosshair */
   u32 r = p->getCVarI(sc_xhair_r); if(r > 255) r = 255;
   u32 g = p->getCVarI(sc_xhair_g); if(g > 255) g = 255;
   u32 b = p->getCVarI(sc_xhair_b); if(b > 255) b = 255;
   k32 a = p->getCVarI(sc_xhair_a); if(a > 255) a = 255; a /= 255.0k;

   if(p->getCVarI(sc_xhair_enable) && a > 0.0k) {
      StrAry(xb, s":XHairs:circleb", s":XHairs:deltab",  s":XHairs:ovalb",
                 s":XHairs:basicb",  s":XHairs:delearb", s":XHairs:finib",
                 s":XHairs:angleb",  s":XHairs:dotb",    s":XHairs:xpb");
      StrAry(xw, s":XHairs:circlew", s":XHairs:deltaw",  s":XHairs:ovalw",
                 s":XHairs:basicw",  s":XHairs:delearw", s":XHairs:finiw",
                 s":XHairs:anglew",  s":XHairs:dotw",    s":XHairs:xpw");

      u32 c     = (r << 16) | (g << 8) | (b << 0);
      i32 style = p->getCVarI(sc_xhair_style);
      PrintSpriteA (xb[style], xh.x,0, xh.y,0, a);
      PrintSpriteAC(xw[style], xh.x,0, xh.y,0, a, c);

      if(p->getCVarI(sc_xhair_enablejuicer)) {
         Str(sp_xhairs_l, s":XHairs:L");
         Str(sp_xhairs_r, s":XHairs:R");

         i32 xp = ceilk(p->extrpitch * 500.0k) + 10;
         PrintSpriteAC(sp_xhairs_l, xh.x - xp,0, xh.y,0, a, c);
         PrintSpriteAC(sp_xhairs_r, xh.x + xp,0, xh.y,0, a, c);
      }
   }
}

alloc_aut(0) script
void P_TeleportIn(struct player *p) {
   p->teleportedout = false;

   ACS_AmbientSound(ss_misc_telein, 127);
   ACS_SetCameraToTexture(p->tid, sp_lithcam3, 90);

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
void P_TeleportOut(struct player *p, i32 tag) {
   ACS_AmbientSound(ss_misc_teleout, 127);
   ACS_SetCameraToTexture(p->tid, sp_lithcam3, 90);

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

   p->teleportedout = true;

   ACS_Teleport_NewMap(tag, 0, false);
}

/* EOF */
