#include "lith_common.h"
#include "lith_weapons.h"
#include "lith_player.h"
#include "lith_hudid.h"

#include <math.h>

//----------------------------------------------------------------------------
// Extern Functions
//

[[__call("ScriptS"), __extern("ACS")]]
int Lith_UniqueTID(void)
{
   return ACS_UniqueTID();
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_Timer(void)
{
   return ACS_Timer();
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_UpdateScore(void)
{
   score_t score = ACS_CheckInventory("Lith_ScoreCount") * RandomFloat(.5, 2);
   Lith_GiveAllScore(score, false);
   ACS_TakeInventory("Lith_ScoreCount", 0x7FFFFFFF);
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_SwitchRifleFiremode(void)
{
   player_t *p = LocalPlayer;
   int max = rifle_firemode_max;
   
   if(!p->upgrades[UPGR_RifleModes].active)
      max--;
   
   p->riflefiremode = (++p->riflefiremode) % max;
   ACS_LocalAmbientSound("weapons/rifle/firemode", 127);
}

[[__call("ScriptS"), __extern("ACS")]]
fixed Lith_Velocity(fixed velx, fixed vely)
{
   fixed x = ACS_GetActorX(0);
   fixed y = ACS_GetActorY(0);
   return ACS_FixedSqrt((x * x) + (y * y));
}

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

[[__call("ScriptS"), __extern("ACS")]]
bool Lith_CheckHealth(int n)
{
   player_t *p = LocalPlayer;
   p->health = ACS_GetActorProperty(0, APROP_Health);
   return p->health < p->maxhealth;
}

[[__call("ScriptS"), __extern("ACS")]]
bool Lith_CheckArmor(int n)
{
   player_t *p = LocalPlayer;
   p->armor = ACS_CheckInventory("BasicArmor");
   return p->maxarmor < n || p->armor == 0 || p->maxarmor == 0 || p->armor < n;
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_Discount()
{
   LocalPlayer->discount = 0.9;
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_DOGS()
{
   player_t *p = LocalPlayer;
   
   int tid = ACS_UniqueTID();
   
   ACS_SetMusic("lmusic/DOGS.ogg");
   
   for(int i = 0; i < (35 * 30) / 10; i++)
   {
      fixed ang = ACS_RandomFixed(0, 1);
      fixed dst = ACS_RandomFixed(0, 64);
      ACS_SpawnForced("Lith_Steggles", p->x + cosk(ang) * dst, p->y + sink(ang) * dst, p->z + 8, tid);
      ACS_GiveInventory("Lith_Alerter", 1);
      ACS_Delay(10);
   }
   
   ACS_Delay(35);
   
   ACS_SetMusic("lsounds/Silence.flac");
   
   ACS_SetActorProperty(tid, APROP_MasterTID, p->tid);
   ACS_SetActorState(tid, "PureSteggleEnergy");
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_SteggleEnergy()
{
   player_t *p = &players[Lith_GetPlayerNumber(0, AAPTR_FRIENDPLAYER)];
   
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

[[__call("ScriptS"), __extern("ACS")]]
void Lith_Barrier()
{
   player_t *p = LocalPlayer;
   
   for(int i = 0; p->active && i < 35 * 30; i++)
   {
      ACS_GiveInventory("Lith_BarrierSpell", 1);
      ACS_Delay(1);
   }
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_BarrierBullets()
{
   player_t *p = &players[Lith_GetPlayerNumber(0, AAPTR_TARGET)];
   
   fixed ang   = ACS_VectorAngle(ACS_GetActorX(0) - p->x, ACS_GetActorY(0) - p->y);
   fixed xang  = ACS_VectorAngle(p->x - ACS_GetActorX(0), p->y - ACS_GetActorY(0));
   fixed zdiff = p->z - ACS_GetActorZ(0);
   fixed s     = ACS_Sin(ang) * 48.0;
   fixed c     = ACS_Cos(ang) * 48.0;
   fixed z     = (p->z + p->viewheight / 2) - (zdiff / 2);
   
   ACS_SpawnForced("Lith_BarrierFX", p->x + c, p->y + s, z);
   ACS_LineAttack(p->tid, ang + ACS_RandomFixed(-0.1, 0.1), PITCH_BASE + ACS_RandomFixed(0.45, 0.55), 10);
}

[[__call("ScriptS"), __extern("ACS")]]
bool Lith_BarrierCheck()
{
   ACS_SetActivatorToTarget(0);
   return ACS_CheckFlag(0, "COUNTKILL");
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_PoisonFXTicker()
{
   for(int i = 0; i < 17; i++)
   {
      ACS_Delay(1);
      
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

[[__call("ScriptS"), __extern("ACS")]]
void Lith_ResetRifleMode()
{
   player_t *p = LocalPlayer;
   
   if(Lith_GetPCVarInt(p, "lith_weapons_riflemodeclear"))
      p->riflefiremode = rifle_firemode_auto;
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_StepSpeed()
{
   player_t *p = LocalPlayer;
   
	fixed vel = ACS_VectorLength(absk(p->velx), absk(p->vely));
   fixed num = 1k - (vel / 24k);
	fixed mul = minmax(num, 0.35k, 1k);
   
	return 6 * (mul + 0.6k);
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_BoughtItemPickup(int id)
{
   player_t *p = LocalPlayer;
   
   if(id)
   {
      upgrade_t *upgr = &p->upgrades[id];
      Lith_UpgrSetOwned(p, upgr);
      Lith_UpgrToggle(p, upgr);
   }
   else
      p->itemsbought++;
}

#if 0
[[__call("ScriptS"), __extern("ACS")]]
void Lith_GetSigil()
{
   ACS_SetResultValue(1);
   
   player_t *p = LocalPlayer;
   
   if(p->cbi.open)
      Lith_KeyOpenCBI();
   
   p->sigil.acquired = true;
   
   if(ACS_GetCVar("__lith_debug_on"))
      return 1;
   
   p->frozen++;
   ACS_GiveInventory("Lith_TimeHax", 1);
   ACS_GiveInventory("Lith_TimeHax2", 1);
   
   p->setVel(0.0, 0.0, 0.0, false, true);
   ACS_FadeTo(0, 0, 0, 0.4, TICSECOND * 3);
   
   ACS_Delay(3);
   
   __str title_text = "D I V I S I O N  S I G I L";
   
   __str subtitle_text = "Warning: This item is unfathomably dangerous.\n"
                  "                Use only at the expense of your world.";
   
   int title_len = ACS_StrLen(title_text);
   int subtitle_len = ACS_StrLen(subtitle_text);
   
   fixed title_time = (TICSECOND * 5) * title_len;
   fixed subtitle_time = (TICSECOND * 2) * subtitle_len;
   
   HudMessageF("DBIGFONT", title_text);
   HudMessageParams(HUDMSG_TYPEON, hid_sigil_title, CR_ORANGE, 0.5, 0.45, 1.0 + subtitle_time, TICSECOND * 5, 0.3);
   
   HudMessageF("SMALLFNT", subtitle_text);
   HudMessageParams(HUDMSG_TYPEON, hid_sigil_subtitle, CR_RED, 0.5, 0.5, 1.0 + title_time, TICSECOND * 2, 0.3);
   
   ACS_Delay((subtitle_time + title_time + 1.0) * 35.0);
   
   ACS_FadeTo(0, 0, 0, 0.0, TICSECOND * 4);
   
   ACS_TakeInventory("PowerTimeFreezer", 1);
   ACS_TakeInventory("Lith_TimeHax2", 1);
   p->frozen--;
}
#endif

// EOF

