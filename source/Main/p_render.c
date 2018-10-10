// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_world.h"

#include <math.h>

// Static Functions ----------------------------------------------------------|

static void HUD_StringStack(struct player *p);
static void HUD_Waves(struct player *p);

// Extern Functions ----------------------------------------------------------|

//
// Lith_PlayerDebugStats
//
script
void Lith_PlayerDebugStats(struct player *p)
{
   if(!(world.dbgLevel & log_devh)) return;

   SetSize(450, 350);

   ACS_SetFont("LTRMFONT");
   ACS_BeginPrint();
   for(int i = 0; i < dbgstatnum; i++)
      ACS_PrintString(dbgstat[i]);
   for(int i = 0; i < dbgnotenum; i++)
      ACS_PrintString(dbgnote[i]);
   PrintText("confont", CR_WHITE, 10,1, 20,1);
}

//
// Lith_PlayerFootstep
//
script
void Lith_PlayerFootstep(struct player *p)
{
   static struct {__str nam, snd; int nxt;} const stepsnd[] = {
      {"FWATER1", "player/stepw", 11},
      {"FWATER2", "player/stepw", 11},
      {"FWATER3", "player/stepw", 11},
      {"FWATER4", "player/stepw", 11},
      {"BLOOD1",  "player/stepw", 15},
      {"BLOOD2",  "player/stepw", 15},
      {"BLOOD3",  "player/stepw", 15},
      {"NUKAGE1", "player/steps", 15},
      {"NUKAGE2", "player/steps", 15},
      {"NUKAGE3", "player/steps", 15},
      {"SLIME01", "player/steps", 12},
      {"SLIME02", "player/steps", 12},
      {"SLIME03", "player/steps", 12},
      {"SLIME04", "player/steps", 12},
      {"SLIME05", "player/steps", 12},
      {"SLIME06", "player/steps", 12},
      {"SLIME07", "player/steps", 12},
      {"SLIME08", "player/steps", 12},
      {"LAVA1",   "player/stepl", 17},
      {"LAVA2",   "player/stepl", 17},
      {"LAVA3",   "player/stepl", 17},
      {"LAVA4",   "player/stepl", 17},
   };

   if(Lith_IsPaused) return;

   if(p->nextstep) {p->nextstep--; return;}

   fixed dstmul = absk(p->getVel()) / 24.0;
   fixed vol = p->getCVarK("lith_player_footstepvol") * min(dstmul, 1);

   __str floor = ACS_GetActorFloorTexture(0);
   __str snd   = p->stepnoise;
   int   next  = 10;

   if(vol && p->onground)
   {
      for(int i = 0; i < countof(stepsnd); i++)
         if(floor == stepsnd[i].nam) {snd = stepsnd[i].snd; next = stepsnd[i].nxt; break;}

      ACS_PlaySound(p->cameratid, snd, CHAN_BODY, vol);
      p->nextstep = next;
   }
}

//
// Lith_PlayerItemFx
//
stkcall
void Lith_PlayerItemFx(struct player *p)
{
   bool hasir = InvNum("PowerLightAmp");

   if(!hasir && p->hadinfrared)
      ACS_LocalAmbientSound("player/infraredoff", 127);

   p->hadinfrared = hasir;
}

//
// Lith_PlayerDamageBob
//
// Update view bobbing when you get damaged.
//
script
void Lith_PlayerDamageBob(struct player *p)
{
   if(Lith_IsPaused) return;

   if(!InvNum("PowerStrength") && p->health < p->oldhealth)
   {
      fixed64 angle = (fixed64)ACS_RandomFixed(tau, -tau);
      fixed64 distance;

      if(p->bobyaw + p->bobpitch > 0.05)
         angle = lerplk(angle, atan2f(p->bobpitch, p->bobyaw), 0.25lk);

      distance  = mag2lk(p->bobyaw, p->bobpitch);
      distance += (p->oldhealth - p->health) / (fixed64)p->maxhealth;
      distance *= 0.2lk;

      fixed64 ys = sinf(angle), yc = cosf(angle);
      p->bobyaw   = ys * distance;
      p->bobpitch = yc * distance;
   }

   if(p->bobpitch) p->bobpitch = lerplk(p->bobpitch, 0.0, 0.1);
   if(p->bobyaw  ) p->bobyaw   = lerplk(p->bobyaw,   0.0, 0.1);
}

//
// Lith_PlayerView
//
// Update additive view.
//
script
void Lith_PlayerView(struct player *p)
{
   if(Lith_IsPaused) return;

   fixed64 addp = 0, addy = 0;

   if(p->getCVarI("lith_player_damagebob"))
   {
      fixed64 bobmul = p->getCVarK("lith_player_damagebobmul");
      addp += p->bobpitch * bobmul;
      addy += p->bobyaw   * bobmul;
   }

   if(p->extrpitch) p->extrpitch = lerplk(p->extrpitch, 0.0lk, 0.1lk);
   if(p->extryaw  ) p->extryaw   = lerplk(p->extryaw,   0.0lk, 0.1lk);

   p->addpitch = addp + p->extrpitch;
   p->addyaw   = addy + p->extryaw;

   ifauto(fixed, mul, p->getCVarK("lith_player_viewtilt") * 0.2)
   {
           if(p->sidev  ) p->addroll = lerplk(p->addroll, -p->sidev * mul, 0.10);
      else if(p->addroll) p->addroll = lerplk(p->addroll, 0,               0.14);
   }

   DebugStat("exp: lv.%u %u/%u\n", p->attr.level, p->attr.exp, p->attr.expnext);
   DebugStat("x: %k\ny: %k\nz: %k\n", p->x, p->y, p->z);
   DebugStat("vx: %k\nvy: %k\nvz: %k\nvel: %k\n", p->velx, p->vely, p->velz, p->getVel());
   DebugStat("a.y: %k\na.p: %k\na.r: %k\n", p->yaw * 360, p->pitch * 360, p->roll * 360);
   DebugStat("ap.y: %lk\nap.p: %lk\nap.r: %lk\n", p->addyaw * 360, p->addpitch * 360, p->addroll * 360);
}

//
// Lith_PlayerStyle
//
script
void Lith_PlayerStyle(struct player *p)
{
   if(p->scopetoken) {
      ACS_SetActorProperty(0, APROP_RenderStyle, STYLE_Subtract);
      ACS_SetActorPropertyFixed(0, APROP_Alpha, p->getCVarK("lith_weapons_scopealpha") * p->alpha);
   } else {
      ACS_SetActorProperty(0, APROP_RenderStyle, STYLE_Translucent);
      ACS_SetActorPropertyFixed(0, APROP_Alpha, p->getCVarK("lith_weapons_alpha") * p->alpha);
   }
}

//
// Lith_PlayerHUD
//
script
void Lith_PlayerHUD(struct player *p)
{
   ACS_SetHudSize(320, 200);

   if(p->old.scopetoken && !p->scopetoken)
   {
      p->hudstrlist.free(true);

      for(int i = hid_scope_clearS; i <= hid_scope_clearE; i++)
      {
         HudMessage("");
         HudMessagePlain(i, 0.0, 0.0, 0.0);
      }
   }

   if(p->scopetoken)
   {
      if(p->pclass == pcl_cybermage)
      {
         int time = (ACS_Timer() % 16) / 4;
         DrawSpriteXX(StrParam(":HUD_C:ScopeOverlay%i", time + 1),
            HUDMSG_ADDBLEND|HUDMSG_FADEOUT|HUDMSG_ALPHA, hid_scope_overlayE + time, 0.1, 0.1, 0.1, 0.25, 0.5);

         for(int i = 0; i < 200; i++)
            DrawSpriteXX(":HUD:H_D41", HUDMSG_ADDBLEND|HUDMSG_FADEOUT|HUDMSG_ALPHA,
               hid_scope_lineE + i, 32, i+.1, 0.1, 0.1, ACS_RandomFixed(0.3, 0.6));
      }
      else if(p->pclass == pcl_informant)
      {
         fixed a = (1 + ACS_Sin(ACS_Timer() / 70.0)) * 0.25 + 0.5;
         DrawSpriteX(":HUD_I:ScopeOverlay",
            HUDMSG_ADDBLEND|HUDMSG_ALPHA, hid_scope_overlayE, -2.1, -2.1,
            TICSECOND, a);
      }
      else
      {
         HUD_Waves(p);
         HUD_StringStack(p);
      }
   }
}

//
// Lith_PlayerLevelup
//
script
void Lith_PlayerLevelup(struct player *p)
{
   if(p->old.attr.level && p->old.attr.level < p->attr.level) {
      ACS_LocalAmbientSound("player/levelup", 127);
      p->logH(1, Language("LITH_LOG_LevelUp%S", p->discrim), ACS_Random(1000, 9000));
   }
   if(p->attr.lvupstr) {
      PrintTextStr(p->attr.lvupstr);
      PrintText("cnfont", CR_WHITE, 7,1, 17,1);
   }
}

// Static Functions ----------------------------------------------------------|

//
// HUD_StringStack
//
static void HUD_StringStack(struct player *p)
{
   typedef struct hudstr_s
   {
      __str str;
      list_t link;
   } hudstr_t;

   if((ACS_Timer() % 3) == 0)
   {
      hudstr_t *hudstr = Salloc(hudstr_t);
      hudstr->link.construct(hudstr);
      hudstr->str = StrParam("%.8X", ACS_Random(0, 0x7FFFFFFF));

      hudstr->link.link(&p->hudstrlist);

      if(p->hudstrlist.size == 20)
         Dalloc(p->hudstrlist.next->unlink());
   }

   SetSize(320, 200);

   size_t i = 0;
   for(list_t *rover = p->hudstrlist.prev; rover != &p->hudstrlist; rover = rover->prev, i++)
   {
      hudstr_t *hudstr = rover->object;
      PrintTextStr(hudstr->str);
      PrintTextA("confont", CR_RED, 300,2, 20+i*9,1, 0.5);
   }
}

//
// HUD_Waves
//
static void HUD_Waves(struct player *p)
{
   fixed health = (fixed)p->health / (fixed)p->maxhealth;
   int frame = minmax(health * 4, 1, 5);
   int timer = ACS_Timer();
   int pos;

   ACS_SetHudSize(320, 200);

   // Sine (health)
   pos = (10 + timer) % 160;
   DrawSpriteFade(StrParam(":HUD:H_D1%i", frame),
      hid_scope_sineS - pos,
      300.1 + roundk(ACS_Sin(pos / 32.0) * 7.0, 0),
      25.1 + pos,
      1.5, 0.3);

   // Square
   {
      fixed a = ACS_Cos(pos / 32.0);

      pos = (7 + timer) % 160;
      DrawSpriteFade(roundk(a, 2) != 0.0 ? ":HUD:H_D16" : ":HUD:H_D46",
         hid_scope_squareS - pos,
         300.1 + (a >= 0) * 7.0,
         25.1 + pos,
         1.9, 0.1);
   }

   // Triangle
   pos = (5 + timer) % 160;
   DrawSpriteFade(":HUD:H_D14", hid_scope_triS - pos, 300.1 + abs((pos % 16) - 8), 25.1 + pos, 1.2, 0.2);
}

// EOF

