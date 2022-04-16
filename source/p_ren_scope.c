// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "p_hudid.h"

noinit static
str hudstrs[20];

noinit static
mem_size_t hudstrnum;

static
void StringStack(void) {
   if(ACS_Timer() % 3 == 0) {
      str s = strp(ACS_PrintHex(ACS_Random(0x10000000, 0x7FFFFFFF)));
      if(hudstrnum == 20) {
         fastmemmove(&hudstrs[0], &hudstrs[1], sizeof(str) * 19);
         hudstrs[19] = s;
      } else {
         hudstrs[hudstrnum++] = s;
      }
   }

   SetSize(320, 200);

   for(mem_size_t i = 0; i < hudstrnum; ++i) {
      PrintTextA_str(hudstrs[i], sf_ltrmfont, CR_RED, 300,2, 20+i*9,1, 0.5);
   }
}

static
void Waves(void) {
   k32 health = pl.health / (k32)pl.maxhealth;
   i32 frame  = clampi(health * 4, 1, 5);
   i32 timer  = ACS_Timer();

   SetSize(320, 200);

   /* Sine (health) */
   for(i32 i = 0; i < 70; i++) {
      i32 pos = (10 + timer + i) % 160;
      PrintSpriteA(sa_fgfx[frame - 1], 300 + ACS_Sin(pos / 32.0) * 7,1, 25 + pos,1, i / 70.0k);
   }

   /* Square */
   for(i32 i = 0; i < 70; i++) {
      #define FRQ 0.025k

      i32 pos = (7 + timer + i) % 160;
      i32 a   =
         2.0k * (2.0k * floork(FRQ * pos) - floork(2.0k * FRQ * pos)) + 1.0k;

      if(pos % 20) {
         PrintSpriteA(sp_HUD_H_D16, 300 + (a > 0) * 7,1, 25 + pos,1, i / 70.0k);
      } else {
         PrintSpriteA(sp_HUD_H_D46, 307,1, 25 + pos,1, i / 70.0k);
      }
   }

   /* Triangle */
   for(i32 i = 0; i < 70; i++) {
      i32 pos = (5 + timer + i) % 160;
      PrintSpriteA(sp_HUD_H_D14, 300 + fastabs(pos % 16 - 8),1, 25 + pos,1, i / 70.0k);
   }
}

static
void ScopeC(void) {
   i32 which = ACS_Timer() % 16 / 4;

   if(pl.scopetoken) {
      SetFadeA(fid_scopecoS + which, 1, 16, 0.5);

      for(i32 i = 0; i < 200; i++) {
         k32 rn = ACS_RandomFixed(0.3, 0.6);
         SetFadeA(fid_scopecgS + i, 1, 16, rn);
      }
   }

   SetSize(320, 200);

   for(i32 i = 0; i < 4; i++) {
      i32 fid = fid_scopecoS + i;
      if(CheckFade(fid)) PrintSpriteFP(sa_scopes[i], 0,1, 0,1, fid);
   }

   for(i32 i = 0; i < 200; i++) {
      i32 fid = fid_scopecgS + i;
      if(CheckFade(fid)) PrintSpriteFP(sp_HUD_H_D41, 32,0, i,1, fid);
   }
}

static
void ScopeI(void) {
   k32 a = (1 + ACS_Sin(ACS_Timer() / 70.0)) * 0.25 + 0.5;
   SetSize(320, 200);
   PrintSpriteAP(sp_HUD_I_ScopeOverlay, 160,0, 100,0, a);
}

static
void ScopeM(void) {
   Waves();
   StringStack();
}

script void P_Ren_Scope(void) {
   if(pl.old.scopetoken && !pl.scopetoken) {
      hudstrnum = 0;
   }

   switch(pl.pclass) {
      case pcl_cybermage:                   ScopeC(); break;
      case pcl_informant: if(pl.scopetoken) ScopeI(); break;
      case pcl_marine:    if(pl.scopetoken) ScopeM(); break;
   }
}

/* EOF */
