/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
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

/* Extern Functions -------------------------------------------------------- */

void P_Ren_Debug(struct player *p);
void P_Ren_Infrared(struct player *p);
void P_Ren_Magic(struct player *p);
script void P_Ren_Step(struct player *p);
void P_Ren_View(struct player *p);
script void P_Ren_Scope(struct player *p);

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
      PrintTextChS(p->attr.lvupstr);
      PrintText(s_smallfnt, CR_WHITE, 220,1, 75,1);
   }
}

static void P_Ren_Flashlight(struct player *p) {
   if(Paused) return;

   i32 bat_life  = p->getCVarI(sc_light_battery) * 35;
   i32 bat_regen = p->getCVarI(sc_light_regen);

   if(p->old.lt_on != p->lt_on) {
      Str(snd_off, s"player/lightoff");
      Str(snd_on,  s"player/lighton");

      ACS_LocalAmbientSound(p->lt_on ? snd_on : snd_off, 127);

      if(p->old.lt_on) {
         p->lt_target = 0.0;
         p->lt_speed  = 0.05;
      } else {
         p->lt_target = 1.0;
         p->lt_speed  = 0.15;
      }
   }

   if(bat_life != 0) {
      if(p->lt_on) {p->lt_battery--;}
      else         {p->lt_battery += bat_regen;}

      if(p->lt_battery > bat_life) {
         p->lt_battery = bat_life;
      } else if(p->lt_battery < 0) {
         p->lt_battery = 0;
         p->lt_on = false;
      } else if(p->lt_battery < bat_life / 4 && ticks % 10 == 0) {
         p->lt_intensity = ACS_RandomFixed(0.25, 0.75);
      }

      if(p->hudenabled) {
         i32 y = p->lt_battery / (k32)bat_life * 8;

         Str(bar,  s":HUD:Battery");
         Str(back, s":HUD:BatteryOutline");

         SetSize(320, 240);
         PrintSprite(back, 90,1, 238,2);
         SetClip(92, 236 - y, 2, 8);
         PrintSprite(bar, 92,1, 236,2);
         ClearClip();
      }
   }

   if(p->lt_intensity != p->lt_target) {
      p->lt_intensity = lerplk(p->lt_intensity, p->lt_target, p->lt_speed);
   }

   Dbg_Stat("lt_target: %lk\nlt_intensity: %lk\nlt_speed: %lk\n"
            "lt_battery: %u\n",
            p->lt_target, p->lt_intensity, p->lt_speed, p->lt_battery);
}

/* Extern Functions -------------------------------------------------------- */

stkcall void P_Ren_PTickPst(struct player *p) {
   P_Ren_Magic(p);
   P_Ren_Flashlight(p);
   P_Ren_Step(p);
   P_Ren_Infrared(p);
   P_Ren_View(p);
   P_Ren_Scope(p);
   P_Ren_Style(p);
   P_Ren_Advice(p);
   P_Ren_LevelUp(p);
   P_Ren_Debug(p);
}

sync void P_TeleportIn(struct player *p) {
   p->teleportedout = false;

   ACS_AmbientSound(ss_misc_telein, 127);
   ACS_SetCameraToTexture(p->tid, s_LITHCAM3, 90);

   for(i32 i = 18, j = 18; i >= 1; i--) {
      ACS_Delay(1);
      k32 w = 1 + (i / 18.lk / 3.lk * 50);
      k32 h = 1 + (j / 18.lk / 8.lk * 10);
      SetSize(320, 200);
      PrintSprite(sp_Terminal_Teleport, 160,0, 100,0);
      SetSize(640 / w, 480 * h);
      PrintSprite(s_LITHCAM3, 320/w,0, 240*h,0);
      if(i & 3) j--;
   }
}

sync void P_TeleportOut(struct player *p) {
   ACS_AmbientSound(ss_misc_teleout, 127);
   ACS_SetCameraToTexture(p->tid, s_LITHCAM3, 90);

   for(i32 i = 1, j = 1; i <= 18; i++) {
      ACS_Delay(1);
      k32 w = 1 + (i / 18.lk / 3.lk * 50);
      k32 h = 1 + (j / 18.lk / 8.lk * 10);
      SetSize(320, 200);
      PrintSprite(sp_Terminal_Teleport, 160,0, 100,0);
      SetSize(640 / w, 480 * h);
      PrintSprite(s_LITHCAM3, 320/w,0, 240*h,0);
      if(i & 3) j++;
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
