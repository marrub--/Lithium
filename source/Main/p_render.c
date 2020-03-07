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

#include <math.h>

/* Static Functions -------------------------------------------------------- */

static void StringStack(struct player *p)
{
   struct hudstr {str s; list link;};

   if(ACS_Timer() % 3 == 0)
   {
      struct hudstr *hudstr = Salloc(struct hudstr);
      ListCtor(&hudstr->link, hudstr);
      hudstr->s = StrParam("%.8X", ACS_Random(INT32_MIN + 1, INT32_MAX));

      hudstr->link.link(&p->hudstrlist);

      if(p->hudstrlist.size() == 20) Dalloc(p->hudstrlist.next->unlink());
   }

   SetSize(320, 200);

   size_t i = 0;
   for_list_back_it(struct hudstr *hudstr, p->hudstrlist, i++)
      PrintTextA_str(hudstr->s, s_ltrmfont, CR_RED, 300,2, 20+i*9,1, 0.5);
}

static void Waves(struct player *p)
{
   static str const fs[] = {s":HUD:H_D11", s":HUD:H_D12", s":HUD:H_D13",
                            s":HUD:H_D14", s":HUD:H_D15"};

   k32 health = p->health / (k32)p->maxhealth;
   i32 frame  = minmax(health * 4, 1, 5);
   i32 timer  = ACS_Timer();

   SetSize(320, 200);

   /* Sine (health) */
   for(i32 i = 0; i < 70; i++) {
      i32 pos = (10 + timer + i) % 160;
      PrintSpriteA(fs[frame - 1], 300 + roundk(ACS_Sin(pos / 32.0) * 7.0, 0),1, 25 + pos,1, i / 70.0k);
   }

   /* Square */
   for(i32 i = 0; i < 70; i++) {
      i32 pos = (7 + timer + i) % 160;
      k32 a   = ACS_Cos(pos / 32.0);
      PrintSpriteA(roundk(a, 2) != 0.0 ? sp_HUD_H_D16 : sp_HUD_H_D46, 300 + (a >= 0) * 7.0,1, 25 + pos,1, i / 70.0k);
   }

   /* Triangle */
   for(i32 i = 0; i < 70; i++) {
      i32 pos = (5 + timer + i) % 160;
      PrintSpriteA(sp_HUD_H_D14, 300 + fastabs(pos % 16 - 8),1, 25 + pos,1, i / 70.0k);
   }
}

static void ScopeC(struct player *p)
{
   i32 which = ACS_Timer() % 16 / 4;

   if(p->scopetoken) {
      SetFadeA(fid_scopecoS + which, 1, 16, 0.5);

      for(i32 i = 0; i < 200; i++) {
         k32 rn = ACS_RandomFixed(0.3, 0.6);
         SetFadeA(fid_scopecgS + i, 1, 16, rn);
      }
   }

   static str const os[] = {s":HUD_C:ScopeOverlay1", s":HUD_C:ScopeOverlay2",
                            s":HUD_C:ScopeOverlay3", s":HUD_C:ScopeOverlay4"};

   SetSize(320, 200);

   for(u32 i = 0; i < 4; i++) {
      u32 fid = fid_scopecoS + i;
      if(CheckFade(fid)) PrintSpriteFP(os[i], 0,1, 0,1, fid);
   }

   for(u32 i = 0; i < 200; i++) {
      u32 fid = fid_scopecgS + i;
      if(CheckFade(fid)) PrintSpriteFP(sp_HUD_H_D41, 32,0, i,1, fid);
   }
}

static void ScopeI(struct player *p)
{
   k32 a = (1 + ACS_Sin(ACS_Timer() / 70.0)) * 0.25 + 0.5;
   SetSize(320, 200);
   PrintSpriteAP(sp_HUD_I_ScopeOverlay, 160,0, 100,0, a);
}

static void ScopeM(struct player *p)
{
   Waves(p);
   StringStack(p);
}

stkcall
static void DebugStats(struct player *p)
{
   if(!(dbglevel & log_devh)) return;

   SetSize(320, 240);

   ACS_BeginPrint();
   for(i32 i = 0; i < dbgstatnum; i++)
      ACS_PrintString(dbgstat[i]);
   for(i32 i = 0; i < dbgnotenum; i++)
      ACS_PrintString(dbgnote[i]);
   PrintTextA(s_smallfnt, CR_WHITE, 10,1, 20,1, 0.2);
}

script
static void Footstep(struct player *p)
{
   static struct {str nam, snd; i32 nxt;} const stepsnd[] = {
      {s"FWATER1", s"player/stepw", 11},
      {s"FWATER2", s"player/stepw", 11},
      {s"FWATER3", s"player/stepw", 11},
      {s"FWATER4", s"player/stepw", 11},
      {s"BLOOD1",  s"player/stepw", 15},
      {s"BLOOD2",  s"player/stepw", 15},
      {s"BLOOD3",  s"player/stepw", 15},
      {s"NUKAGE1", s"player/steps", 15},
      {s"NUKAGE2", s"player/steps", 15},
      {s"NUKAGE3", s"player/steps", 15},
      {s"SLIME01", s"player/steps", 12},
      {s"SLIME02", s"player/steps", 12},
      {s"SLIME03", s"player/steps", 12},
      {s"SLIME04", s"player/steps", 12},
      {s"SLIME05", s"player/steps", 12},
      {s"SLIME06", s"player/steps", 12},
      {s"SLIME07", s"player/steps", 12},
      {s"SLIME08", s"player/steps", 12},
      {s"LAVA1",   s"player/stepl", 17},
      {s"LAVA2",   s"player/stepl", 17},
      {s"LAVA3",   s"player/stepl", 17},
      {s"LAVA4",   s"player/stepl", 17},
   };

   if(Paused) return;

   if(p->nextstep) {p->nextstep--; return;}

   k32 dstmul = absk(p->getVel()) / 24.0;
   k32 vol    = p->getCVarK(sc_player_footstepvol) * min(dstmul, 1);

   str floor = ACS_GetActorFloorTexture(0);
   str snd   = p->stepnoise;
   i32 next  = 10;

   if(vol && p->onground)
   {
      for(i32 i = 0; i < countof(stepsnd); i++)
         if(floor == stepsnd[i].nam) {snd = stepsnd[i].snd; next = stepsnd[i].nxt; break;}

      ACS_SetActivator(p->cameratid);
      StartSound(snd, lch_body, 0, vol);
      p->nextstep = next;
   }
}

#if LITHIUM
stkcall
static void ItemFx(struct player *p)
{
   bool hasir = InvNum(so_PowerLightAmp);

   if(!hasir && p->hadinfrared)
      ACS_LocalAmbientSound(ss_player_infraredoff, 127);

   p->hadinfrared = hasir;
}
#endif

/* Update view bobbing when you get damaged. */
stkcall
static void DamageBob(struct player *p)
{
   if(Paused) return;

   if(!InvNum(so_PowerStrength) && p->health < p->oldhealth)
   {
      k64 angle = (k64)ACS_RandomFixed(tau, -tau);
      k64 distance;

      if(p->bobyaw + p->bobpitch > 0.05)
         angle = lerplk(angle, atan2f(p->bobpitch, p->bobyaw), 0.25lk);

      distance  = mag2lk(p->bobyaw, p->bobpitch);
      distance += (p->oldhealth - p->health) / (k64)p->maxhealth;
      distance *= 0.2lk;

      k64 ys = sinf(angle), yc = cosf(angle);
      p->bobyaw   = ys * distance;
      p->bobpitch = yc * distance;
   }

   if(p->bobpitch) p->bobpitch = lerplk(p->bobpitch, 0.0, 0.1);
   if(p->bobyaw  ) p->bobyaw   = lerplk(p->bobyaw,   0.0, 0.1);
}

/* Update additive view. */
stkcall
static void View(struct player *p)
{
   if(Paused) return;

   k64 addp = 0, addy = 0;

   if(p->getCVarI(sc_player_damagebob))
   {
      k64 bobmul = p->getCVarK(sc_player_damagebobmul);
      addp += p->bobpitch * bobmul;
      addy += p->bobyaw   * bobmul;
   }

   if(p->extrpitch) p->extrpitch = lerplk(p->extrpitch, 0.0lk, 0.1lk);
   if(p->extryaw  ) p->extryaw   = lerplk(p->extryaw,   0.0lk, 0.1lk);

   p->addpitch = addp + p->extrpitch;
   p->addyaw   = addy + p->extryaw;

   ifauto(k32, mul, p->getCVarK(sc_player_viewtilt) * 0.2)
   {
           if(p->sidev  ) p->addroll = lerplk(p->addroll, -p->sidev * mul, 0.10);
      else if(p->addroll) p->addroll = lerplk(p->addroll, 0,               0.14);
   }

   Dbg_Stat("exp: lv.%u %u/%u\n", p->attr.level, p->attr.exp, p->attr.expnext);
   Dbg_Stat("x: %k\ny: %k\nz: %k\n", p->x, p->y, p->z);
   Dbg_Stat("vx: %k\nvy: %k\nvz: %k\nvel: %k\n", p->velx, p->vely, p->velz, p->getVel());
   Dbg_Stat("a.y: %k\na.p: %k\na.r: %k\n", p->yaw * 360, p->pitch * 360, p->roll * 360);
   Dbg_Stat("ap.y: %lk\nap.p: %lk\nap.r: %lk\n", p->addyaw * 360, p->addpitch * 360, p->addroll * 360);
   Dbg_Stat("rage: %k\n", p->rage);
   Dbg_Stat("score: %lli\n", p->score);
}

#if LITHIUM
stkcall
static void Style(struct player *p)
{
   if(p->scopetoken) {
      SetPropI(0, APROP_RenderStyle, STYLE_Subtract);
      SetPropK(0, APROP_Alpha, p->getCVarK(sc_weapons_scopealpha) * p->alpha);
   } else {
      SetPropI(0, APROP_RenderStyle, STYLE_Translucent);
      SetPropK(0, APROP_Alpha, p->getCVarK(sc_weapons_alpha) * p->alpha);
   }
}

static void Advice(struct player *p)
{
   /*
   SetSize(320, 240);
   PrintTextChS(p->attr.lvupstr);
   PrintText(s_cnfont, CR_WHITE, 220,1, 75,1);
   */
}

script
static void HUD(struct player *p)
{
   if(p->old.scopetoken && !p->scopetoken)
      ListDtor(&p->hudstrlist, true);

   switch(p->pclass) {
      case pcl_cybermage:                   ScopeC(p); break;
      case pcl_informant: if(p->scopetoken) ScopeI(p); break;
      case pcl_marine:    if(p->scopetoken) ScopeM(p); break;
   }
}
#endif

stkcall
static void Levelup(struct player *p)
{
   if(p->old.attr.level && p->old.attr.level < p->attr.level) {
      ACS_LocalAmbientSound(ss_player_levelup, 127);
      p->logH(1, LanguageC(LANG "LOG_LevelUp%s", p->discrim), ACS_Random(1000, 9000));
   }

   if(p->attr.lvupstr[0]) {
      SetSize(320, 240);
      PrintTextChS(p->attr.lvupstr);
      PrintText(s_cnfont, CR_WHITE, 220,1, 75,1);
   }
}

/* Extern Functions -------------------------------------------------------- */

stkcall
void P_Ren_PTickPst(struct player *p)
{
   Footstep(p);
   #if LITHIUM
   ItemFx(p);
   #endif
   DamageBob(p);
   View(p);
   #if LITHIUM
   HUD(p);
   Style(p);
   Advice(p);
   #endif
   Levelup(p);
   DebugStats(p);
}

sync void P_TeleportIn(struct player *p)
{
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

sync void P_TeleportOut(struct player *p)
{
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

script void P_TeleportInAsync(struct player *p)
{
   P_TeleportIn(p);
}

script void P_TeleportOutAsync(struct player *p)
{
   P_TeleportOut(p);
}

/* EOF */
