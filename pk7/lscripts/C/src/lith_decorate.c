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
   static __str const pickupsounds[] = {
      [weapon_unknown] = "MMMMHMHMMMHMMM",
      [weapon_pistol] = "weapons/pistol/pickup",
      [weapon_shotgun] = "weapons/shotgun/pickup",
      [weapon_supershotgun] = "weapons/ssg/pickup",
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
      __str const *names = pickupnames[user_pickupparm];
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
int Lith_SuperOmegaRandom(int min, int max)
{
   return Random(max, min);
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
      
      A = ACS_Sin(a) * dx;
      P = ACS_Cos(a) * dy;
      
      return bitsk(A);
   }
   else
      return bitsk(P);
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_UniqueTID()
{
   return ACS_UniqueTID();
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_GetCVar(int var)
{
   switch(var)
   {
   case lith_weapons_zoomfactor:
      return bitsk(ACS_GetCVarFixed("lith_weapons_zoomfactor"));
   }
   
   return -1;
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_UpdateScore()
{
   player_t *p = &players[ACS_PlayerNumber()];
   
   double rmul = RandomFloat(1.0f, 6.0f);
   score_t score = ACS_CheckInventory("Lith_ScoreCount") * rmul;
   
   if(ACS_GetCVar("lith_player_scoresound"))
      ACS_LocalAmbientSound("player/score", minmax(score / 5000.0f, 0.1f, 1.0f) * 80);
   
   p->score += score;
   p->scoresum += score;
   p->scoreaccum += score;
   p->scoreaccumtime = 35 * rmul;
   
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
   else if(ACS_PlayerNumber() == -1)
      return;
   
   ACS_GiveInventory("Lith_ScoreCount", amount);
   Lith_UpdateScore();
}

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
void Lith_SwitchRifleFiremode()
{
   player_t *p = &players[ACS_PlayerNumber()];
   p->riflefiremode = (p->riflefiremode + 1) % rifle_firemode_max;
   ACS_LocalAmbientSound("weapons/rifle/firemode", 127);
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_GetRifleFiremode()
{
   player_t *p = &players[ACS_PlayerNumber()];
   return p->riflefiremode;
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_PistolBulletTrace(bool hitactor)
{
   fixed angle = ACS_GetActorAngle(0);
   fixed pitch = ACS_GetActorPitch(0);
   
   int user_timesshot = ACS_GetUserVariable(0, "user_timesshot");
   int user_thingshit = ACS_GetUserVariable(0, "user_thingshit");
   int user_timesshot_max = ACS_GetCVar("lith_sv_ricochet_max");
   
   if(user_timesshot_max > 9999)
      ACS_ActivatorSound("MMMMHMHMMMHMMM", 127);
   else
      ACS_ActivatorSound("effects/puff/ricochet", 127);
   
   angle = absk(1.0 - angle);
   angle += ACS_RandomFixed(-0.2, 0.2);
   angle = absk(angle);
   
   ACS_Delay(2);
   
   int tid = ACS_UniqueTID();
   ACS_Thing_ChangeTID(0, tid);
   
   if(ACS_GetCVar("lith_sv_ricochet") && user_timesshot < user_timesshot_max)
   {
      int pufftid = ACS_UniqueTID();
      
      ACS_LineAttack(0, angle, pitch, 20 / (user_thingshit + 1), "Lith_PistolPuff",
         "PlayerMissile", 0.0, FHF_NORANDOMPUFFZ, pufftid);
      
      if(!hitactor)
         user_timesshot++;
      else
         user_thingshit++;
      
      ACS_SetUserVariable(pufftid, "user_timesshot", user_timesshot);
      ACS_SetUserVariable(pufftid, "user_thingshit", user_thingshit);
      
      ACS_SetActivatorToTarget(0);
      int playertid = ACS_ActivatorTID();
      
      if(ACS_GetCVar("lith_player_pong") && hitactor && user_thingshit >= 2)
         ACS_LocalAmbientSound(StrParam("player/pong%i", min(user_thingshit, 3)), 50);
      
      ACS_SetActivator(pufftid);
      ACS_SetPointer(AAPTR_TARGET, playertid);
      
      ACS_SetActivator(tid);
      
      fixed puffx = ACS_GetActorX(pufftid);
      fixed puffy = ACS_GetActorY(pufftid);
      fixed puffz = ACS_GetActorZ(pufftid);
      
      fixed x = ACS_GetActorX(0);
      fixed y = ACS_GetActorY(0);
      fixed z = ACS_GetActorZ(0);
      
      for(int i = 0; i < 35; i++)
      {
         x = lerpk(x, puffx, 0.1);
         y = lerpk(y, puffy, 0.1);
         z = lerpk(z, puffz, 0.1);
         
         ACS_SpawnForced("Lith_PistolTrace", x, y, z);
      }
   }
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_SpawnWeaponText(int user_pickupparm, int user_spritetid)
{
   if(ACS_GetCVar("lith_sv_noweapontext"))
      return;
   
   fixed x = ACS_GetActorX(0);
   fixed y = ACS_GetActorY(0);
   fixed z = ACS_GetActorZ(0);
   
   __str str;
   
   if(ACS_GetCVar("lith_sv_stupidpickups"))
   {
      int namesmax;
      
      for(namesmax = 1; pickupnames[user_pickupparm][namesmax]; namesmax++);
      
      str = StrUpper(pickupnames[user_pickupparm][ACS_Random(1, namesmax - 1)]);
   }
   else
      str = StrUpper(pickupnames[user_pickupparm][0]);
   
   int len = ACS_StrLen(str);
   for(int i = 0; str[i]; i++)
   {
      if(str[i] != ' ')
      {
         int temptid = ACS_UniqueTID();
         ACS_SpawnForced("Lith_WeaponText", x, y, z, temptid);
         ACS_SetUserVariable(temptid, "user_char", str[i]);
         ACS_SetUserVariable(temptid, "user_dist", (len / 2 - i) * 8);
         
         int mastertid = ACS_ActivatorTID();
         ACS_SetActivator(temptid);
         ACS_SetPointer(AAPTR_MASTER, mastertid);
         ACS_Thing_ChangeTID(0, user_spritetid);
         ACS_SetActivator(mastertid);
      }
   }
}

//
// ---------------------------------------------------------------------------


