/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "p_hudid.h"

static void StringStack(struct player *p) {
   struct hudstr {str s; list link;};

   if(ACS_Timer() % 3 == 0) {
      struct hudstr *hudstr = Salloc(struct hudstr, _tag_huds);
      ListCtor(&hudstr->link, hudstr);
      hudstr->s = StrParam("%.8X", ACS_Random(INT32_MIN + 1, INT32_MAX));

      hudstr->link.link(&p->hudstrlist);

      if(p->hudstrlist.size() == 20) Dalloc(p->hudstrlist.next->unlink());
   }

   SetSize(320, 200);

   size_t i = 0;
   for_list_back_it(struct hudstr *hudstr, p->hudstrlist, i++)
      PrintTextA_str(hudstr->s, sf_ltrmfont, CR_RED, 300,2, 20+i*9,1, 0.5);
}

static void Waves(struct player *p) {
   StrAry(fs, s":HUD:H_D11", s":HUD:H_D12", s":HUD:H_D13", s":HUD:H_D14",
              s":HUD:H_D15");

   k32 health = p->health / (k32)p->maxhealth;
   i32 frame  = minmax(health * 4, 1, 5);
   i32 timer  = ACS_Timer();

   SetSize(320, 200);

   /* Sine (health) */
   for(i32 i = 0; i < 70; i++) {
      i32 pos = (10 + timer + i) % 160;
      PrintSpriteA(fs[frame - 1], 300 + fastroundk(ACS_Sin(pos / 32.0) * 7.0, 0),1, 25 + pos,1, i / 70.0k);
   }

   /* Square */
   for(i32 i = 0; i < 70; i++) {
      i32 pos = (7 + timer + i) % 160;
      k32 a   = ACS_Cos(pos / 32.0);
      PrintSpriteA(fastroundk(a, 2) != 0.0 ? sp_HUD_H_D16 : sp_HUD_H_D46, 300 + (a >= 0) * 7.0,1, 25 + pos,1, i / 70.0k);
   }

   /* Triangle */
   for(i32 i = 0; i < 70; i++) {
      i32 pos = (5 + timer + i) % 160;
      PrintSpriteA(sp_HUD_H_D14, 300 + fastabs(pos % 16 - 8),1, 25 + pos,1, i / 70.0k);
   }
}

static void ScopeC(struct player *p) {
   i32 which = ACS_Timer() % 16 / 4;

   if(p->scopetoken) {
      SetFadeA(fid_scopecoS + which, 1, 16, 0.5);

      for(i32 i = 0; i < 200; i++) {
         k32 rn = ACS_RandomFixed(0.3, 0.6);
         SetFadeA(fid_scopecgS + i, 1, 16, rn);
      }
   }

   StrAry(os, s":HUD_C:ScopeOverlay1", s":HUD_C:ScopeOverlay2",
              s":HUD_C:ScopeOverlay3", s":HUD_C:ScopeOverlay4");

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

static void ScopeI(struct player *p) {
   Str(sp_HUD_I_ScopeOverlay, s":HUD_I:ScopeOverlay");

   k32 a = (1 + ACS_Sin(ACS_Timer() / 70.0)) * 0.25 + 0.5;
   SetSize(320, 200);
   PrintSpriteAP(sp_HUD_I_ScopeOverlay, 160,0, 100,0, a);
}

static void ScopeM(struct player *p) {
   Waves(p);
   StringStack(p);
}

script void P_Ren_Scope(struct player *p) {
   if(p->old.scopetoken && !p->scopetoken)
      ListDtor(&p->hudstrlist, true);

   switch(p->pclass) {
      case pcl_cybermage:                   ScopeC(p); break;
      case pcl_informant: if(p->scopetoken) ScopeI(p); break;
      case pcl_marine:    if(p->scopetoken) ScopeM(p); break;
   }
}

/* EOF */
