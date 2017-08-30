#include "lith_common.h"
#include "lith_weapons.h"
#include "lith_player.h"
#include "lith_hudid.h"
#include "lith_world.h"

#include <math.h>

// How to make DECORATE into a decent VM: Registers!

#define PVarFunc(fn, expr) \
   [[__call("ScriptS"), __extern("ACS")]] \
   int fn \
   { \
      withplayer(LocalPlayer) { \
         int *val = &p->decvars[var - 1]; \
         return expr; \
      } \
      return 0; \
   }

#define WVarFunc(fn, expr) \
   [[__call("ScriptS"), __extern("ACS")]] \
   int fn {int *val = &world.decvars[var - 1]; return expr;}

#define RegisterMachine(reg, name) \
   reg(L##name##Get(int var         ), (*val)       ) \
   reg(L##name##Inc(int var         ), (*val)++     ) \
   reg(L##name##Dec(int var         ), (*val)--     ) \
   reg(L##name##Set(int var, int num), (*val)  = num) \
   reg(L##name##Add(int var, int num), (*val) += num) \
   reg(L##name##Sub(int var, int num), (*val) -= num) \
   reg(L##name##Mul(int var, int num), (*val) *= num) \
   reg(L##name##Div(int var, int num), (*val) /= num) \
   reg(L##name##Mod(int var, int num), (*val) %= num) \
   reg(L##name##Min(int var, int num), (*val) = min(*val, num)) \
   reg(L##name##Max(int var, int num), (*val) = max(*val, num))

RegisterMachine(PVarFunc, PVar)
RegisterMachine(WVarFunc, WVar)

//
// Lith_UniqueTID
//
[[__call("ScriptS"), __extern("ACS")]]
int Lith_UniqueTID(void)
{
   return ACS_UniqueTID();
}

//
// Lith_Timer
//
[[__call("ScriptS"), __extern("ACS")]]
int Lith_Timer(void)
{
   return ACS_Timer();
}

//
// Lith_UpdateScore
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_UpdateScore(void)
{
   score_t score = ACS_CheckInventory("Lith_ScoreCount") * (double)RandomFloat(0.5, 1.5);
   Lith_GiveAllScore(score, false);
   ACS_TakeInventory("Lith_ScoreCount", 0x7FFFFFFF);
}

//
// Lith_Velocity
//
[[__call("ScriptS"), __extern("ACS")]]
fixed Lith_Velocity(fixed velx, fixed vely)
{
   fixed x = ACS_GetActorX(0);
   fixed y = ACS_GetActorY(0);
   return ACS_FixedSqrt((x * x) + (y * y));
}

//
// Lith_VelHax
//
[[__call("ScriptS"), __extern("ACS")]]
int Lith_VelHax(int fuck)
{
   ACS_SetActivator(0, AAPTR_MASTER);

   switch(fuck)
   {
   case 1: return ACS_GetActorVelX(0);
   case 2: return ACS_GetActorVelY(0);
   case 3: return ACS_GetActorVelZ(0);
   }

   return -1;
}

//
// Lith_CheckHealth
//
[[__call("ScriptS"), __extern("ACS")]]
bool Lith_CheckHealth(int n)
{
   withplayer(LocalPlayer) {
      p->health = ACS_GetActorProperty(0, APROP_Health);
      return p->health < p->maxhealth;
   }
   return 0;
}

//
// Lith_CheckArmor
//
[[__call("ScriptS"), __extern("ACS")]]
bool Lith_CheckArmor(int n)
{
   withplayer(LocalPlayer) {
      p->armor = ACS_CheckInventory("BasicArmor");
      return p->maxarmor  < n || p->armor == 0 ||
             p->maxarmor == 0 || p->armor  < n;
   }
   return 0;
}

//
// Lith_Discount
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_Discount()
{
   withplayer(LocalPlayer)
      p->discount = 0.9;
}

//
// Lith_DOGS
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_DOGS()
{
   player_t *p = LocalPlayer;
   if(NoPlayer(p)) return;

   int tid = ACS_UniqueTID();

   ACS_SetMusic("lmusic/DOGS.ogg");

   for(int i = 0; i < (35 * 30) / 10; i++)
   {
      fixed ang = ACS_RandomFixed(0, 1);
      fixed dst = ACS_RandomFixed(0, 64);
      ACS_SpawnForced("Lith_Steggles", p->x + ACS_Cos(ang) * dst, p->y + ACS_Sin(ang) * dst, p->z + 8, tid);
      ACS_GiveInventory("Lith_Alerter", 1);
      ACS_Delay(10);
   }

   ACS_Delay(35);

   ACS_SetMusic("lsounds/Silence.flac");

   ACS_SetActorProperty(tid, APROP_MasterTID, p->tid);
   ACS_SetActorState(tid, "PureSteggleEnergy");
}

//
// Lith_SteggleEnergy
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_SteggleEnergy()
{
   player_t *p = Lith_GetPlayer(0, AAPTR_FRIENDPLAYER);

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

//
// Lith_Barrier
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_Barrier()
{
   withplayer(LocalPlayer)
      for(int i = 0; p->active && i < 35 * 30; i++)
   {
      ACS_GiveInventory("Lith_BarrierSpell", 1);
      ACS_Delay(1);
   }
}

//
// Lith_BarrierBullets
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_BarrierBullets()
{
   player_t *p = Lith_GetPlayer(0, AAPTR_TARGET);

   fixed ang   = ACS_VectorAngle(ACS_GetActorX(0) - p->x, ACS_GetActorY(0) - p->y);
   fixed xang  = ACS_VectorAngle(p->x - ACS_GetActorX(0), p->y - ACS_GetActorY(0));
   fixed zdiff = p->z - ACS_GetActorZ(0);
   fixed s     = ACS_Sin(ang) * 48.0;
   fixed c     = ACS_Cos(ang) * 48.0;
   fixed z     = (p->z + p->viewheight / 2) - (zdiff / 2);

   ACS_SpawnForced("Lith_BarrierFX", p->x + c, p->y + s, z);
   ACS_LineAttack(p->tid, ang + ACS_RandomFixed(-0.1, 0.1), PITCH_BASE + ACS_RandomFixed(0.45, 0.55), 10);
}

//
// Lith_BarrierCheck
//
[[__call("ScriptS"), __extern("ACS")]]
bool Lith_BarrierCheck()
{
   ACS_SetActivatorToTarget(0);
   return ACS_CheckFlag(0, "COUNTKILL");
}

//
// Lith_PoisonFXTicker
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_PoisonFXTicker()
{
   for(int i = 0; i < 17; i++)
   {
      do
         ACS_Delay(1);
      while(Lith_ScriptCall("Lith_PauseManager", "GetPaused"));

      if(ACS_CheckInventory("Lith_PoisonFXReset"))
      {
         ACS_TakeInventory("Lith_PoisonFXReset", 999);
         ACS_TakeInventory("Lith_PoisonFXTimer", 999);
         ACS_GiveInventory("Lith_PoisonFXGiverGiver", 1);
         return;
      }
   }

   if(ACS_GetActorProperty(0, APROP_Health) <= 0)
   {
      ACS_TakeInventory("Lith_PoisonFXReset", 999);
      ACS_TakeInventory("Lith_PoisonFXTimer", 999);
      ACS_TakeInventory("Lith_PoisonFXGiverGiver", 999);
      return;
   }

   if(ACS_CheckInventory("Lith_PoisonFXTimer"))
   {
      ACS_GiveInventory("Lith_PoisonFXGiver", 1);
      ACS_TakeInventory("Lith_PoisonFXTimer", 1);
   }
}

//
// Lith_BoughtItemPickup
//
[[__call("ScriptI"), __address(24243), __extern("ACS")]]
void Lith_BoughtItemPickup(int id)
{
   int const chan = CHAN_ITEM|CHAN_NOPAUSE;
   player_t *p = LocalPlayer;
   if(NoPlayer(p)) return;

   if(id)
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
[[__call("ScriptS"), __extern("ACS")]]
void Lith_GetSigil()
{
   player_t *p = LocalPlayer;
   if(NoPlayer(p)) return;

   p->closeGUI();

   p->sigil.acquired = true;

   ACS_Thing_Remove(ACS_CheckInventory("Lith_DivisionSigilSpriteTID"));
   ACS_TakeInventory("Lith_DivisionSigilSpriteTID", 0x7FFFFFFF);

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

// EOF

