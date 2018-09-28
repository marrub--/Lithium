// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_weapons.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_world.h"

//
// Lith_Timer
//
script ext("ACS")
int Lith_Timer(void)
{
   return ACS_Timer();
}

//
// Lith_AddAngleScript
//
script addr("Lith_AddAngle")
void Lith_AddAngleScript(int x, int y)
{
   Lith_AddAngle(x, y);
}

//
// Lith_EmitScore
//
script ext("ACS")
void Lith_EmitScore(int amount)
{
   Lith_GiveAllScore(amount, false);
}

//
// Lith_EmitEXP
//
script ext("ACS")
void Lith_EmitEXP(int amount)
{
   Lith_GiveAllEXP(amount);
}

//
// Lith_GiveScoreScript
//
script ext("ACS") addr("Lith_GiveScore")
void Lith_GiveScoreScript(int score)
{
   Lith_GiveAllScore(score * (fixed64)ACS_RandomFixed(0.7, 1.2), false);
}

//
// Lith_DOGS
//
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

//
// Lith_SteggleEnergy
//
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

//
// Lith_Barrier
//
script ext("ACS")
void Lith_Barrier()
{
   withplayer(LocalPlayer) for(int i = 0; p->active && i < 35 * 30; i++)
   {
      InvGive("Lith_BarrierSpell", 1);
      ACS_Delay(1);
   }
}

//
// Lith_BarrierBullets
//
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

//
// Lith_BarrierCheck
//
script ext("ACS")
bool Lith_BarrierCheck()
{
   ACS_SetActivatorToTarget(0);
   return ACS_CheckFlag(0, "COUNTKILL");
}

//
// Lith_PoisonFXTicker
//
script ext("ACS")
void Lith_PoisonFXTicker()
{
   for(int i = 0; i < 17; i++)
   {
      Lith_PausableTick();

      if(InvNum("Lith_PoisonFXReset"))
      {
         InvTake("Lith_PoisonFXReset", 999);
         InvTake("Lith_PoisonFXTimer", 999);
         InvGive("Lith_PoisonFXGiverGiver", 1);
         return;
      }
   }

   if(ACS_GetActorProperty(0, APROP_Health) <= 0)
   {
      InvTake("Lith_PoisonFXReset", 999);
      InvTake("Lith_PoisonFXTimer", 999);
      InvTake("Lith_PoisonFXGiverGiver", 999);
   }
   else if(InvNum("Lith_PoisonFXTimer"))
   {
      InvGive("Lith_PoisonFXGiver", 1);
      InvTake("Lith_PoisonFXTimer", 1);
   }
}

//
// Lith_BoughtItemPickup
//
script ext("ACS")
void Lith_BoughtItemPickup(int id)
{
   int const chan = CHAN_ITEM|CHAN_NOPAUSE;
   withplayer(LocalPlayer) if(id)
   {
      upgrade_t *upgr = p->getUpgr(id);

      if(!upgr->owned)
      {
         switch(upgr->info->category)
         {
         case UC_Body: ACS_PlaySound(0, "player/pickup/upgrbody", chan, 1, false, ATTN_NONE); break;
         case UC_Weap: ACS_PlaySound(0, "player/pickup/upgrweap", chan, 1, false, ATTN_NONE); break;
         case UC_Extr: ACS_PlaySound(0, "player/pickup/upgrextr", chan, 1, false, ATTN_NONE); break;
         }

         upgr->setOwned(p);
         upgr->toggle(p);
      }
   }
   else
   {
      ACS_PlaySound(0, "player/pickup/item", chan, 1, false, ATTN_NONE);

      p->itemsbought++;
   }
}

//
// Lith_GetSigil
//
script ext("ACS")
void Lith_GetSigil()
{
   withplayer(LocalPlayer)
   {
      p->closeGUI();

      p->sigil.acquired = true;

      ACS_Thing_Remove(InvNum("Lith_DivisionSigilSpriteTID"));
      InvTake("Lith_DivisionSigilSpriteTID", 0x7FFFFFFF);

      if(world.dbgLevel)
         return;

      world.freeze(true);

      ACS_FadeTo(0, 0, 0, 0.4, TICSECOND * 3);
      ACS_SetHudSize(320, 200);

      ACS_Delay(3);

      HudMessageF("DBIGFONT", "D I V I S I O N  S I G I L");
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

