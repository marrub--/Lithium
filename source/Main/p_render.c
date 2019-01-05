// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#include "common.h"
#include "p_player.h"
#include "p_hudid.h"
#include "w_world.h"

#include <math.h>

// Static Functions ----------------------------------------------------------|

#if LITHIUM
static void ScopeC(struct player *p)
{
   i32 time = (ACS_Timer() % 16) / 4;
   DrawSpriteXX(StrParam(":HUD_C:ScopeOverlay%i", time + 1),
      HUDMSG_ADDBLEND|HUDMSG_FADEOUT|HUDMSG_ALPHA, hid_scope_overlayE + time, 0.1, 0.1, 0.1, 0.25, 0.5);

   for(i32 i = 0; i < 200; i++)
      DrawSpriteXX(sp_HUD_H_D41, HUDMSG_ADDBLEND|HUDMSG_FADEOUT|HUDMSG_ALPHA, hid_scope_lineE + i, 32, i+.1, 0.1, 0.1, ACS_RandomFixed(0.3, 0.6));
}

static void ScopeI(struct player *p)
{
   k32 a = (1 + ACS_Sin(ACS_Timer() / 70.0)) * 0.25 + 0.5;
   DrawSpriteX(sp_HUD_I_ScopeOverlay, HUDMSG_ADDBLEND|HUDMSG_ALPHA, hid_scope_overlayE, -2.1, -2.1, TS, a);
}

static void ScopeM(struct player *p)
{
   static void StringStack(struct player *p);
   static void Waves(struct player *p);

   Waves(p);
   StringStack(p);
}
#endif

// Extern Functions ----------------------------------------------------------|

script
void Lith_PlayerDebugStats(struct player *p)
{
   if(!(world.dbgLevel & log_devh)) return;

   SetSize(320, 240);

   ACS_BeginPrint();
   for(i32 i = 0; i < dbgstatnum; i++)
      ACS_PrintString(dbgstat[i]);
   for(i32 i = 0; i < dbgnotenum; i++)
      ACS_PrintString(dbgnote[i]);
   PrintText(s_smallfnt, CR_WHITE, 10,1, 20,1);
}

script
void Lith_PlayerFootstep(struct player *p)
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

   if(Lith_IsPaused) return;

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

      ACS_PlaySound(p->cameratid, snd, CHAN_BODY, vol);
      p->nextstep = next;
   }
}

#if LITHIUM
stkcall
void Lith_PlayerItemFx(struct player *p)
{
   bool hasir = InvNum(so_PowerLightAmp);

   if(!hasir && p->hadinfrared)
      ACS_LocalAmbientSound(ss_player_infraredoff, 127);

   p->hadinfrared = hasir;
}
#endif

// Update view bobbing when you get damaged.
script
void Lith_PlayerDamageBob(struct player *p)
{
   if(Lith_IsPaused) return;

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

// Update additive view.
script
void Lith_PlayerView(struct player *p)
{
   if(Lith_IsPaused) return;

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

   DebugStat("exp: lv.%u %u/%u\n", p->attr.level, p->attr.exp, p->attr.expnext);
   DebugStat("x: %k\ny: %k\nz: %k\n", p->x, p->y, p->z);
   DebugStat("vx: %k\nvy: %k\nvz: %k\nvel: %k\n", p->velx, p->vely, p->velz, p->getVel());
   DebugStat("a.y: %k\na.p: %k\na.r: %k\n", p->yaw * 360, p->pitch * 360, p->roll * 360);
   DebugStat("ap.y: %lk\nap.p: %lk\nap.r: %lk\n", p->addyaw * 360, p->addpitch * 360, p->addroll * 360);
   DebugStat("rage: %k\n", p->rage);
}

#if LITHIUM
script
void Lith_PlayerStyle(struct player *p)
{
   if(p->scopetoken) {
      SetPropI(0, APROP_RenderStyle, STYLE_Subtract);
      SetPropK(0, APROP_Alpha, p->getCVarK(sc_weapons_scopealpha) * p->alpha);
   } else {
      SetPropI(0, APROP_RenderStyle, STYLE_Translucent);
      SetPropK(0, APROP_Alpha, p->getCVarK(sc_weapons_alpha) * p->alpha);
   }
}

script
void Lith_PlayerHUD(struct player *p)
{
   ACS_SetHudSize(320, 200);

   if(p->old.scopetoken && !p->scopetoken)
   {
      p->hudstrlist.free(true);

      for(i32 i = hid_scope_clearS; i <= hid_scope_clearE; i++)
      {
         ACS_BeginPrint();
         ACS_MoreHudMessage();
         HudMessagePlain(i, 0.0, 0.0, 0.0);
      }
   }

   if(p->scopetoken) switch(p->pclass) {
   case pcl_cybermage: ScopeC(p); break;
   case pcl_informant: ScopeI(p); break;
   case pcl_marine:    ScopeM(p); break;
   }
}
#endif

script
void Lith_PlayerLevelup(struct player *p)
{
   if(p->old.attr.level && p->old.attr.level < p->attr.level)
   {
      ACS_LocalAmbientSound(ss_player_levelup, 127);
      p->logH(1, LanguageC(LANG "LOG_LevelUp%s", p->discrim), ACS_Random(1000, 9000));
   }

   if(p->attr.lvupstr[0])
   {
      SetSize(320, 240);
      PrintTextChS(p->attr.lvupstr);
      PrintText(s_cnfont, CR_WHITE, 7,1, 17,1);
   }
}

// Static Functions ----------------------------------------------------------|

#if LITHIUM
static void StringStack(struct player *p)
{
   struct hudstr {str s; list link;};

   if(ACS_Timer() % 3 == 0)
   {
      struct hudstr *hudstr = Salloc(struct hudstr);
      hudstr->link.construct(hudstr);
      hudstr->s = StrParam("%.8X", ACS_Random(0, 0x7FFFFFFF));

      hudstr->link.link(&p->hudstrlist);

      if(p->hudstrlist.size() == 20)
         Dalloc(p->hudstrlist.next->unlink());
   }

   SetSize(320, 200);

   size_t i = 0;
   for_list_back_it(struct hudstr *hudstr, p->hudstrlist, i++)
      PrintTextA_str(hudstr->s, s_confont, CR_RED, 300,2, 20+i*9,1, 0.5);
}

static void Waves(struct player *p)
{
   k32 health = p->health / (k32)p->maxhealth;
   i32 frame  = minmax(health * 4, 1, 5);
   i32 timer  = ACS_Timer();

   ACS_SetHudSize(320, 200);

   // Sine (health)
   i32 pos = (10 + timer) % 160;
   DrawSpriteFade(StrParam(":HUD:H_D1%i", frame),
      hid_scope_sineS - pos,
      300.1 + roundk(ACS_Sin(pos / 32.0) * 7.0, 0),
      25.1 + pos,
      1.5, 0.3);

   // Square
   k32 a = ACS_Cos(pos / 32.0);

   pos = (7 + timer) % 160;
   DrawSpriteFade(roundk(a, 2) != 0.0 ? sp_HUD_H_D16 : sp_HUD_H_D46,
      hid_scope_squareS - pos,
      300.1 + (a >= 0) * 7.0,
      25.1 + pos,
      1.9, 0.1);

   // Triangle
   pos = (5 + timer) % 160;
   DrawSpriteFade(sp_HUD_H_D14, hid_scope_triS - pos, 300.1 + abs((pos % 16) - 8), 25.1 + pos, 1.2, 0.2);
}
#endif

// EOF
