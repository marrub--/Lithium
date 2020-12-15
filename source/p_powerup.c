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
void Sc_DOGS(void)
{
   with_player(LocalPlayer)
   {
      i32 tid = ACS_UniqueTID();

      ACS_SetMusic(sp_lmusic_DOGS);

      for(i32 i = 0; i < (35 * 30) / 10; i++)
      {
         k32 ang = ACS_RandomFixed(0, 1);
         k32 dst = ACS_RandomFixed(0, 64);
         ACS_SpawnForced(so_Steggles, p->x + ACS_Cos(ang) * dst, p->y + ACS_Sin(ang) * dst, p->z + 8, tid);
         ServCallI(sm_AlertMonsters);
         ACS_Delay(10);
      }

      ACS_Delay(35);

      ACS_SetMusic(sp_lsounds_Silence);

      SetMasterTID(tid, p->tid);
      ACS_SetActorState(tid, sm_PureSteggleEnergy);
   }
}

alloc_aut(0) script_str ext("ACS") addr(OBJ "SteggleEnergy")
void Sc_SteggleEnergy(void)
{
   with_player(P_PtrFind(0, AAPTR_FRIENDPLAYER))
   {
      ACS_SetPointer(AAPTR_TARGET, 0, AAPTR_FRIENDPLAYER);

      for(;;)
      {
         k32 x = GetX(0);
         k32 y = GetY(0);
         k32 z = GetZ(0);

         k32 nx = lerpk(x, p->x, 0.01);
         k32 ny = lerpk(y, p->y, 0.01);
         k32 nz = lerpk(z, p->z, 0.01);

         ACS_Warp(0, nx, ny, nz, 0, WARPF_ABSOLUTEPOSITION|WARPF_NOCHECKPOSITION|WARPF_INTERPOLATE);
         ACS_SetActorAngle(0, ACS_VectorAngle(p->x - x, p->y - y));

         ACS_Delay(1);
      }
   }
}

script_str ext("ACS") addr(OBJ "BarrierBullets")
void Sc_BarrierBullets(void)
{
   with_player(P_PtrFind(0, AAPTR_TARGET))
   {
      k32 ang   = ACS_VectorAngle(GetX(0) - p->x, GetY(0) - p->y);
      k32 xang  = ACS_VectorAngle(p->x - GetX(0), p->y - GetY(0));
      k32 zdiff = p->z - GetZ(0);
      k32 s     = ACS_Sin(ang) * 48.0;
      k32 c     = ACS_Cos(ang) * 48.0;
      k32 z     = (p->z + p->viewheight / 2) - (zdiff / 2);

      ACS_SpawnForced(so_BarrierFX, p->x + c, p->y + s, z);
      ACS_LineAttack(p->tid, ang + ACS_RandomFixed(-0.1, 0.1), PITCH_BASE + ACS_RandomFixed(0.45, 0.55), 10);
   }
}

dynam_aut script_str ext("ACS") addr(OBJ "GetSigil")
void Sc_GetSigil(void)
{
   with_player(LocalPlayer)
   {
      P_GUI_Close(p);

      p->sgacquired = true;

      #ifndef NDEBUG
      if(dbglevel) return;
      #endif

      FreezeTime();

      ACS_FadeTo(0, 0, 0, 0.4, 0.087);

      ACS_Delay(3);

      SetFade(fid_divsigil, 44, 8);
      for(i32 i = 0;; i++) {
         Str(sl_div_get,     sLANG "DIV_SIGIL_GET");
         Str(sl_div_warning, sLANG "DIV_SIGIL_WARNING");

         if(!CheckFade(fid_divsigil)) break;

         SetSize(640, 480);
         PrintTextF_str(L(sl_div_get), sf_bigupper, CR_ORANGE, 320,4, 240,2, fid_divsigil);
         PrintTextF_str(L(sl_div_warning), sf_bigupper, CR_RED, 320,4, 240,1, fid_divsigil);

         ACS_Delay(1);
      }

      ACS_FadeTo(0, 0, 0, 0.0, 0.145);

      UnfreezeTime();
   }
}

script_str ext("ACS") addr(OBJ "GiveHealthBonus")
void Sc_GiveHealthBonus(i32 amount)
{
   with_player(LocalPlayer)
   {
      amount += p->health;
      if(amount > p->maxhealth + 100) amount = p->maxhealth + 100;
      p->health = amount;
   }
}

script_str ext("ACS") addr(OBJ "GiveHealth")
void Sc_GiveHealth(i32 amount)
{
   with_player(LocalPlayer)
   {
      amount += p->health;
      amount *= 1 + p->attr.attrs[at_vit] / 80.0;
      if(amount > p->maxhealth) amount = p->maxhealth;
      p->health = amount;
   }
}

script_str ext("ACS") addr(OBJ "CheckHealth")
bool Sc_CheckHealth(void)
{
   with_player(LocalPlayer) return p->health < p->maxhealth;
   return 0;
}

script_str ext("ACS") addr(OBJ "Discount")
void Sc_Discount(void)
{
   with_player(LocalPlayer) p->discount = 0.9;
}

/* EOF */
