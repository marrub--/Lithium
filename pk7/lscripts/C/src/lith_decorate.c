#include "lith_common.h"
#include "lith_weapons.h"
#include "lith_cvars.h"
#include "lith_player.h"

// ---------------------------------------------------------------------------
// DECORATE scripts.
//

[[__call("ScriptI"), __address(200), __extern("ACS")]]
void Lith_WeaponPickup(int user_pickupparm, int user_spritetid)
{
   #include "lith_pickups.h"
   
   ACS_Thing_Remove(user_spritetid);
   
   if(ACS_GetCVar("lith_hud_stupidpickups"))
   {
      __str const *names = pickupnames[user_pickupparm];
      int namesmax;
      
      for(namesmax = 1; names[namesmax]; namesmax++);
      
      int i1 = ACS_Random(0, pickupfmtmax - 1);
      int i2 = ACS_Random(1, namesmax - 1);
      int i3 = ACS_Random(0, uncertaintymax - 1);
      
      if(pickupfmt[i1].flag == 1)
         Log(pickupfmt[i1].fmt, names[i2], uncertainty[i3]);
      else
         Log(pickupfmt[i1].fmt, names[i2]);
   }
   else
      Log(pickupfmt[0].fmt, pickupnames[user_pickupparm][0]);
   
   switch(user_pickupparm)
   {
   default:
      ACS_LocalAmbientSound("misc/w_pkup", 127);
      break;
   case weapon_shotgun:
      ACS_LocalAmbientSound("weapons/shotgun/open", 127);
      ACS_Delay(10);
      ACS_LocalAmbientSound("weapons/shotgun/zoomout", 127);
      break;
   case weapon_supershotgun:
      ACS_LocalAmbientSound("weapons/ssg/load", 127);
      ACS_Delay(10);
      ACS_LocalAmbientSound("weapons/ssg/chargefinish", 127);
      break;
   case weapon_combatrifle:
      ACS_LocalAmbientSound("weapons/rifle/pickup", 127);
      break;
   case weapon_rocketlauncher:
      ACS_LocalAmbientSound("weapons/rocket/pickup", 127);
      break;
   case weapon_plasmarifle:
      ACS_LocalAmbientSound("weapons/plasma/pickup", 127);
      break;
   case weapon_bfg9000:
      ACS_LocalAmbientSound("weapons/cannon/pickup", 127);
      break;
   }
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
   
   float rmul = RandomFloat(1.0f, 6.0f);
   score_t score = ACS_CheckInventory("Lith_ScoreCount") * rmul;
   
   p->score += score;
   p->scoreaccum += score;
   p->scoreaccumtime = 35 * rmul;
   
   ACS_TakeInventory("Lith_ScoreCount", 0x7FFFFFFF);
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_GiveScoreToTarget(int amount)
{
   ACS_SetActivatorToTarget(0);
   
   if(ACS_PlayerNumber() == -1)
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
      p->scoreaccumtime = -1;
      return true;
   }
   
   p->score -= amount;
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

//
// ---------------------------------------------------------------------------

