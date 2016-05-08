#include "lith_common.h"
#include "lith_weapons.h"
#include "lith_cvars.h"
#include "lith_player.h"
#include "lith_pickups.h"

// ---------------------------------------------------------------------------
// DECORATE scripts.
//

[[__call("ScriptI"), __address(200), __extern("ACS")]]
void Lith_WeaponPickup(int user_pickupparm, int user_spritetid)
{
   static __str pickupsounds[] = {
      [weapon_unknown] = "MMMMHMHMMMHMMM",
      [weapon_pistol] = "weapons/pistol/pickup",
      [weapon_shotgun] = "weapons/shotgun/pickup",
      [weapon_combatrifle] = "weapons/rifle/pickup",
      [weapon_rocketlauncher] = "weapons/rocket/pickup",
      [weapon_plasmarifle] = "weapons/plasma/pickup",
      [weapon_bfg9000] = "weapons/cannon/pickup"
   };
   
   player_t *p = &players[ACS_PlayerNumber()];
   
   p->weaponsheld++;
   
   ACS_Thing_Remove(user_spritetid);
   ACS_LocalAmbientSound(pickupsounds[user_pickupparm], 127);
   
   if(ACS_GetCVar("lith_sv_stupidpickups"))
   {
      __str *names = pickupnames[user_pickupparm];
      int namesmax;
      
      for(namesmax = 1; names[namesmax]; namesmax++);
      
      int ifmt = ACS_Random(0, pickupfmtmax - 1);
      int iname = ACS_Random(1, namesmax - 1);
      
      if(pickupfmt[ifmt].flag == 2)
         ifmt = ACS_Random(0, pickupfmtmax - 1);
      
      __str fmt = StrParam("\Cj%S", pickupfmt[ifmt].fmt);
      int flag = pickupfmt[ifmt].flag;
      
      if(flag == 1)
         Log(fmt, names[iname], uncertainty[ACS_Random(0, uncertaintymax - 2)]);
      else
         Log(fmt, names[iname], uncertainty[ACS_Random(0, uncertaintymax - 1)]);
   }
   else
      Log(pickupfmt[0].fmt, pickupnames[user_pickupparm][0]);
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_CircleSpread(fixed mdx, fixed mdy, bool getpitch)
{
   static fixed A;
   static fixed P;
   
   if(!getpitch)
   {
      fixed dx = RandomFixed(mdx, 0.0);
      fixed dy = RandomFixed(mdy, 0.0);
      fixed a = RandomFixed(1.0, -1.0);
      
      A = sink(a) * dx;
      P = cosk(a) * dy;
      
      return bitsk(A);
   }
   else
      return bitsk(P);
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_UniqueTID(void)
{
   return ACS_UniqueTID();
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_GetCVar(int var)
{
   switch(var)
   case lith_weapons_zoomfactor:
      return bitsk(ACS_GetCVarFixed("lith_weapons_zoomfactor"));
   
   return -1;
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_UpdateScore(void)
{
   player_t *p = &players[ACS_PlayerNumber()];
   
   double rmul = RandomFloat(1.0f, 6.0f);
   score_t score = ACS_CheckInventory("Lith_ScoreCount") * rmul;
   
   Lith_GiveScore(p, score);
   
   ACS_TakeInventory("Lith_ScoreCount", 0x7FFFFFFF);
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_GiveScoreToTarget(int amount)
{
   ACS_SetActivatorToTarget(0);
   
   if(!ACS_StrICmp(ACS_GetActorClass(0), "Lith_PistolPuff"))
   {
      for(int i = 0, n = min(ACS_GetUserVariable(0, "user_thingshit"), 3); i < n; i++)
         amount *= 1.5f;
      
      ACS_SetActivatorToTarget(0);
   }
   
   if(ACS_PlayerNumber() == -1)
      return;
   
   ACS_GiveInventory("Lith_ScoreCount", amount);
   Lith_UpdateScore();
}

// TODO: replace with shop
[[__call("ScriptS"), __extern("ACS")]]
bool Lith_FireScore(int amount)
{
   player_t *p = &players[ACS_PlayerNumber()];
   
   if(p->score - amount < 0)
   {
      p->scoreaccumtime = -2;
      return true;
   }
   
   p->score -= amount;
   p->scoreused += amount;
   return false;
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_SwitchRifleFiremode(void)
{
   player_t *p = &players[ACS_PlayerNumber()];
   int max = rifle_firemode_max;
   
   if(!p->upgrades[UPGR_RifleModes].active)
      max--;
   
   p->riflefiremode = (++p->riflefiremode) % max;
   ACS_LocalAmbientSound("weapons/rifle/firemode", 127);
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_GetPlayerData(int info)
{
   player_t *p = &players[ACS_PlayerNumber()];
   
   switch(info)
   {
   case pdata_rifle_firemode: return p->riflefiremode;
   case pdata_shotgun_gauss:  return p->upgrades[UPGR_GaussShotty].active;
   case pdata_rocket_unreal:  return p->upgrades[UPGR_ChargeNader].active;
   case pdata_plasma_laser:   return p->upgrades[UPGR_PlasLaser].active;
   case pdata_buttons:        return p->buttons;
   }
   
   return -1;
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_ShotgunHasGauss(void)
{
   player_t *p = &players[ACS_PlayerNumber()];
   return p->upgrades[UPGR_GaussShotty].active;
}

[[__call("ScriptS"), __extern("ACS")]]
fixed Lith_Velocity(fixed velx, fixed vely)
{
   return pymagk(velx, vely);
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_PickupScore(int user_pickupparm, int user_spritetid)
{
   ACS_SetActivatorToTarget(0);
   
   player_t *p = &players[ACS_PlayerNumber()];
   
   if(!(p->weapons & (1 << (user_pickupparm - 1))))
      return true;
   
   ACS_SpawnForced("Lith_FakeItemPickup", p->x, p->y, p->z);
   ACS_SetActorVelocity(0, 0.001, 0, 0, true, false);
   Lith_GiveScore(p, 11100 * user_pickupparm);
   
   ACS_Thing_Remove(user_spritetid);
   
   return false;
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

//
// ---------------------------------------------------------------------------


