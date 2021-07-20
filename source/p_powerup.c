/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Various functions used by powerups.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "p_hudid.h"
#include "w_world.h"

/* Scripts ----------------------------------------------------------------- */

alloc_aut(0) script_str ext("ACS") addr(OBJ "DOGS")
void Sc_DOGS(void) {
   i32 tid = ACS_UniqueTID();

   ACS_SetMusic(sp_lmusic_DOGS);

   for(i32 i = 0; i < (35 * 30) / 10; i++) {
      k32 ang = ACS_RandomFixed(0, 1);
      k32 dst = ACS_RandomFixed(0, 64);
      ACS_SpawnForced(so_Steggles, pl.x + ACS_Cos(ang) * dst, pl.y + ACS_Sin(ang) * dst, pl.z + 8, tid);
      ServCallI(sm_AlertMonsters);
      ACS_Delay(10);
   }

   ACS_Delay(35);
   ACS_SetMusic(sp_lsounds_Silence);
   ACS_SetActorState(tid, sm_PureSteggleEnergy);
}

script_str ext("ACS") addr(OBJ "BarrierBullets")
void Sc_BarrierBullets(void) {
   if(!P_None() && PtrPlayerNumber(0, AAPTR_TARGET) >= 0) {
      k32 ang   = ACS_VectorAngle(GetX(0) - pl.x, GetY(0) - pl.y);
      k32 xang  = ACS_VectorAngle(pl.x - GetX(0), pl.y - GetY(0));
      k32 zdiff = pl.z - GetZ(0);
      k32 s     = ACS_Sin(ang) * 48.0;
      k32 c     = ACS_Cos(ang) * 48.0;
      k32 z     = (pl.z + pl.viewheight / 2) - (zdiff / 2);

      ACS_SpawnForced(so_BarrierFX, pl.x + c, pl.y + s, z);
      ACS_LineAttack(pl.tid, ang + ACS_RandomFixed(-0.1, 0.1), PITCH_BASE + ACS_RandomFixed(0.45, 0.55), 10);
   }
}

dynam_aut script_str ext("ACS") addr(OBJ "GetSigil")
void Sc_GetSigil(void) {
   if(!P_None()) {
      P_GUI_Close();

      pl.sgacquired = true;

      #ifndef NDEBUG
      if(dbglevel_any()) return;
      #endif

      FreezeTime();

      ACS_FadeTo(0, 0, 0, 0.4, 0.087);

      ACS_Delay(3);

      SetFade(fid_divsigil, 44, 8);
      for(i32 i = 0;; i++) {
         if(!CheckFade(fid_divsigil)) break;

         SetSize(640, 480);
         PrintTextF_str(ns(lang(sl_div_get)), sf_bigupper, CR_ORANGE, 320,4, 240,2, fid_divsigil);
         PrintTextF_str(ns(lang(sl_div_warning)), sf_bigupper, CR_RED, 320,4, 240,1, fid_divsigil);

         ACS_Delay(1);
      }

      ACS_FadeTo(0, 0, 0, 0.0, 0.145);

      UnfreezeTime();
   }
}

script_str ext("ACS") addr(OBJ "GiveHealthBonus")
void Sc_GiveHealthBonus(i32 amount)
{
   if(!P_None())
   {
      amount += pl.health;
      if(amount > pl.maxhealth + 100) amount = pl.maxhealth + 100;
      pl.health = amount;
   }
}

script_str ext("ACS") addr(OBJ "GiveHealth")
void Sc_GiveHealth(i32 amount) {
   if(!P_None()) {
      amount += pl.health;
      amount *= 1 + pl.attr.attrs[at_vit] / 80.0;
      if(amount > pl.maxhealth) amount = pl.maxhealth;
      pl.health = amount;
   }
}

script_str ext("ACS") addr(OBJ "CheckHealth")
bool Sc_CheckHealth(void) {
   if(!P_None()) return pl.health < pl.maxhealth;
   return 0;
}

#define set_discount() pl.discount = 0.9

script_str ext("ACS") addr(OBJ "Discount")
void Sc_Discount(void) {
   set_discount();
}

script static
bool chtf_discount(cheat_params_t const params) {
   set_discount();
   return true;
}

struct cheat cht_discount = cheat_s("pgtaxevasion", 0, chtf_discount);

/* EOF */
