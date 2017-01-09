#include "lith_common.h"
#include "lith_weapons.h"
#include "lith_cvars.h"
#include "lith_player.h"
#include "lith_pickups.h"
#include "lith_hudid.h"
#include "lith_hud.h"

//----------------------------------------------------------------------------
// Scripts
//

[[__call("ScriptI"), __address(14242), __extern("ACS")]]
void Lith_WeaponPickup(int user_pickupparm, int user_tid)
{
   static __str pickupsounds[] = {
      [weapon_unknown]  = "MMMMHMHMMMHMMM",
      [weapon_pistol]   = "weapons/pistol/pickup",
      [weapon_shotgun]  = "weapons/shotgun/pickup",
      [weapon_rifle]    = "weapons/rifle/pickup",
      [weapon_launcher] = "weapons/rocket/pickup",
      [weapon_plasma]   = "weapons/plasma/pickup",
      [weapon_bfg]      = "weapons/cannon/pickup"
   };
   
   player_t *p = Lith_LocalPlayer;
   
   if(p->weapons & (1 << user_pickupparm))
      return;
   
   p->weaponsheld++;
   
   if(!ACS_GetCVar("sv_weaponstay"))
      ACS_Thing_Remove(user_tid);
   
   if(!p->upgrades[UPGR_7777777].active)
      ACS_LocalAmbientSound(pickupsounds[user_pickupparm], 127);
   else
      ACS_LocalAmbientSound("marathon/pickup", 127);
   
   switch(user_pickupparm)
   {
   case weapon_pistol:   Lith_UnlockBIPPage(&p->bip, "Pistol");   break;
   case weapon_shotgun:  Lith_UnlockBIPPage(&p->bip, "Shotgun");  break;
   case weapon_rifle:    Lith_UnlockBIPPage(&p->bip, "Rifle");    break;
   case weapon_launcher:
      ACS_GiveInventory("Lith_RocketAmmo", 20);
      Lith_UnlockBIPPage(&p->bip, "Launcher");
      break;
   case weapon_plasma:
      ACS_GiveInventory("Lith_PlasmaAmmo", 2000);
      Lith_UnlockBIPPage(&p->bip, "Plasma");
      break;
   case weapon_bfg:
      ACS_GiveInventory("Lith_CannonAmmo", 2);
      Lith_UnlockBIPPage(&p->bip, "Cannon");
      break;
   }
   
   if(ACS_GetCVar("lith_sv_stupidpickups"))
      Lith_StupidPickup(p, user_pickupparm);
   else
      Lith_IntelligentPickup(p, user_pickupparm);
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
      fixed a  = RandomFixed(1.0, -1.0);
      
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
int Lith_Timer(void)
{
   return ACS_Timer();
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_GetCVar(int var)
{
   switch(var)
   case lith_weapons_zoomfactor:
      return bitsk(ACS_GetUserCVarFixed(ACS_PlayerNumber(), "lith_weapons_zoomfactor"));
   
   return -1;
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_UpdateScore(void)
{
   score_t score = ACS_CheckInventory("Lith_ScoreCount") * RandomFloat(1.0f, 6.0f);
   Lith_GiveAllScore(score);
   ACS_TakeInventory("Lith_ScoreCount", 0x7FFFFFFF);
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_EmitScore(int amount)
{
   Lith_GiveAllScore(amount);
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_SwitchRifleFiremode(void)
{
   player_t *p = Lith_LocalPlayer;
   int max = rifle_firemode_max;
   
   if(!p->upgrades[UPGR_RifleModes].active)
      max--;
   
   p->riflefiremode = (++p->riflefiremode) % max;
   ACS_LocalAmbientSound("weapons/rifle/firemode", 127);
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_GetPlayerData(int info, bool target)
{
   if(target)
      ACS_SetActivatorToTarget(0);
   
   if(ACS_PlayerNumber() < 0)
      return 0;
   
   player_t *p = Lith_LocalPlayer;
   
   switch(info)
   {
   case pdata_rifle_firemode: return p->riflefiremode;
   case pdata_shotgun_gauss:  return p->upgrades[UPGR_GaussShotty].active;
   case pdata_rocket_unreal:  return p->upgrades[UPGR_ChargeRPG].active;
   case pdata_plasma_laser:   return p->upgrades[UPGR_PlasLaser].active;
   case pdata_punctuator:     return p->upgrades[UPGR_Punct].active;
   case pdata_buttons:        return p->buttons;
   case pdata_has_sigil:      return p->sigil.acquired;
   case pdata_EXPLOOOOOSIONS: return p->upgrades[UPGR_TorgueMode].active;
   case pdata_marathon_mode:  return p->upgrades[UPGR_7777777].active;
   case pdata_yh0armor:       return p->upgrades[UPGR_ReactArmour].active;
   }
   
   return 0;
}

[[__call("ScriptS"), __extern("ACS")]]
fixed Lith_Velocity(fixed velx, fixed vely)
{
   return pymagk(velx, vely);
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_PickupScore(int user_pickupparm, int user_spritetid)
{
   if(ACS_GetCVar("sv_weaponstay")) return true;
   
   ACS_SetActivatorToTarget(0);
   
   player_t *p = Lith_LocalPlayer;
   
   if(!(p->weapons & (1 << user_pickupparm)))
      return true;
   
   Lith_Log(p, "> You sold the weapon for Score.");
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

[[__call("ScriptS"), __extern("ACS")]]
int Lith_Oscillate()
{
   static bool x;
   return x = !x;
}

[[__call("ScriptS"), __extern("ACS")]]
int Lith_OscillateN(int n)
{
   static int x;
   return x %= n, x++;
}

[[__call("ScriptS"), __extern("ACS")]]
bool Lith_CheckHealth(int n)
{
   player_t *p = Lith_LocalPlayer;
   return p->health < p->maxhealth;
}

[[__call("ScriptS"), __extern("ACS")]]
bool Lith_CheckArmor(int n)
{
   player_t *p = Lith_LocalPlayer;
   return p->maxarmor < n || p->armor == 0 || p->maxarmor == 0 || p->armor < n;
}

[[__call("ScriptS"), __extern("ACS")]]
void Lith_Discount()
{
   Lith_LocalPlayer->discount = 0.75;
}

#if 0
[[__call("ScriptS"), __extern("ACS")]]
int Lith_GetSigil()
{
   ACS_SetResultValue(1); // q_q
   
   player_t *p = Lith_LocalPlayer;
   
   if(p->cbi.open)
      Lith_KeyOpenCBI();
   
   p->sigil.acquired = true;
   
   if(ACS_GetCVar("__lith_debug_on"))
      return 1;
   
   p->frozen++;
   ACS_GiveInventory("Lith_TimeHax", 1);
   ACS_GiveInventory("Lith_TimeHax2", 1);
   
   Lith_SetPlayerVelocity(p, 0.0, 0.0, 0.0, false, true);
   ACS_FadeTo(0, 0, 0, 0.4, TICSECOND * 3);
   
   ACS_Delay(3);
   
   __str title_text = "D I V I S I O N  S I G I L";
   
   __str subtitle_text = "Warning: This item is unfathomably dangerous.\n"
                  "                Use only at the expense of your world.";
   
   int title_len = ACS_StrLen(title_text);
   int subtitle_len = ACS_StrLen(subtitle_text);
   
   fixed title_time = (TICSECOND * 5) * title_len;
   fixed subtitle_time = (TICSECOND * 2) * subtitle_len;
   
   HudMessageF("BIGFONT", title_text);
   HudMessageParams(HUDMSG_TYPEON, hid_sigil_title, CR_ORANGE, 0.5, 0.45, 1.0 + subtitle_time, TICSECOND * 5, 0.3);
   
   HudMessageF("SMALLFNT", subtitle_text);
   HudMessageParams(HUDMSG_TYPEON, hid_sigil_subtitle, CR_RED, 0.5, 0.5, 1.0 + title_time, TICSECOND * 2, 0.3);
   
   ACS_Delay((subtitle_time + title_time + 1.0) * 35.0);
   
   ACS_FadeTo(0, 0, 0, 0.0, TICSECOND * 4);
   
   ACS_TakeInventory("PowerTimeFreezer", 1);
   ACS_TakeInventory("Lith_TimeHax2", 1);
   p->frozen--;
   
   return 1; // q_q
}
#endif

// EOF

