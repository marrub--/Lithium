// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_weapons.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_world.h"

script ext("ACS")
void Lith_DOGS()
{
   withplayer(LocalPlayer)
   {
      int tid = ACS_UniqueTID();

      ACS_SetMusic("lmusic/DOGS.ogg");

      for(int i = 0; i < (35 * 30) / 10; i++)
      {
         fixed ang = ACS_RandomFixed(0, 1);
         fixed dst = ACS_RandomFixed(0, 64);
         ACS_SpawnForced("Lith_Steggles", p->x + ACS_Cos(ang) * dst, p->y + ACS_Sin(ang) * dst, p->z + 8, tid);
         InvGive("Lith_Alerter", 1);
         ACS_Delay(10);
      }

      ACS_Delay(35);

      ACS_SetMusic("lsounds/Silence.flac");

      ACS_SetActorProperty(tid, APROP_MasterTID, p->tid);
      ACS_SetActorState(tid, "PureSteggleEnergy");
   }
}

script ext("ACS")
void Lith_SteggleEnergy()
{
   withplayer(Lith_GetPlayer(0, AAPTR_FRIENDPLAYER))
   {
      ACS_SetPointer(AAPTR_TARGET, 0, AAPTR_FRIENDPLAYER);

      for(;;)
      {
         fixed x = ACS_GetActorX(0);
         fixed y = ACS_GetActorY(0);
         fixed z = ACS_GetActorZ(0);

         fixed nx = lerpk(x, p->x, 0.01);
         fixed ny = lerpk(y, p->y, 0.01);
         fixed nz = lerpk(z, p->z, 0.01);

         ACS_Warp(0, nx, ny, nz, 0, WARPF_ABSOLUTEPOSITION|WARPF_NOCHECKPOSITION|WARPF_INTERPOLATE);
         ACS_SetActorAngle(0, ACS_VectorAngle(p->x - x, p->y - y));

         ACS_Delay(1);
      }
   }
}

script ext("ACS")
void Lith_Barrier()
{
   withplayer(LocalPlayer) for(int i = 0; p->active && i < 35 * 30; i++)
   {
      InvGive("Lith_BarrierSpell", 1);
      ACS_Delay(1);
   }
}

script ext("ACS")
void Lith_BarrierBullets()
{
   withplayer(Lith_GetPlayer(0, AAPTR_TARGET))
   {
      fixed ang   = ACS_VectorAngle(ACS_GetActorX(0) - p->x, ACS_GetActorY(0) - p->y);
      fixed xang  = ACS_VectorAngle(p->x - ACS_GetActorX(0), p->y - ACS_GetActorY(0));
      fixed zdiff = p->z - ACS_GetActorZ(0);
      fixed s     = ACS_Sin(ang) * 48.0;
      fixed c     = ACS_Cos(ang) * 48.0;
      fixed z     = (p->z + p->viewheight / 2) - (zdiff / 2);

      ACS_SpawnForced("Lith_BarrierFX", p->x + c, p->y + s, z);
      ACS_LineAttack(p->tid, ang + ACS_RandomFixed(-0.1, 0.1), PITCH_BASE + ACS_RandomFixed(0.45, 0.55), 10);
   }
}

script ext("ACS")
bool Lith_BarrierCheck()
{
   ACS_SetActivatorToTarget(0);
   return ACS_CheckFlag(0, "COUNTKILL");
}

script ext("ACS")
void Lith_GetSigil()
{
   withplayer(LocalPlayer)
   {
      p->closeGUI();

      p->sgacquired = true;

      ACS_Thing_Remove(InvNum("Lith_DivisionSigilSpriteTID"));
      InvTake("Lith_DivisionSigilSpriteTID", 0x7FFFFFFF);

      if(world.dbgLevel)
         return;

      world.freeze(true);

      ACS_FadeTo(0, 0, 0, 0.4, TICSECOND * 3);
      ACS_SetHudSize(320, 200);

      ACS_Delay(3);

      HudMessageF("dbigfont", "D I V I S I O N  S I G I L");
      HudMessageParams(HUDMSG_TYPEON, hid_sigil_title, CR_ORANGE, 160.4, 100.2, 1.5, TICSECOND * 5, 0.3);

      HudMessageF("SMALLFNT",
         "=== Warning ===\n"
         "This item is unfathomably dangerous.\n"
         "Use at the expense of your world.");
      HudMessageParams(HUDMSG_TYPEON, hid_sigil_subtitle, CR_RED, 160.4, 100.1, 1.0, TICSECOND * 2, 0.3);

      ACS_Delay(35 * 7);

      ACS_FadeTo(0, 0, 0, 0.0, TICSECOND * 5);

      world.freeze(false);
   }
}

// EOF
