#include "lith_common.h"
#include "lith_weapons.h"
#include "lith_cvars.h"
#include "lith_player.h"
#include "lith_pickups.h"
#include "lith_hudid.h"

//----------------------------------------------------------------------------
// Scripts
//

[[__call("ScriptI"), __address(14242), __extern("ACS")]]
void Lith_WeaponPickup(int user_pickupparm, int user_spritetid)
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
   
   player_t *p = &players[ACS_PlayerNumber()];
   
   p->weaponsheld++;
   
   ACS_Thing_Remove(user_spritetid);
   ACS_LocalAmbientSound(pickupsounds[user_pickupparm], 127);
   
   switch(user_pickupparm)
   {
   case weapon_pistol:   Lith_UnlockBIPPage(&p->bip, "Pistol");   break;
   case weapon_shotgun:  Lith_UnlockBIPPage(&p->bip, "Shotgun");  break;
   case weapon_rifle:    Lith_UnlockBIPPage(&p->bip, "Rifle");    break;
   case weapon_launcher: Lith_UnlockBIPPage(&p->bip, "Launcher"); break;
   case weapon_plasma:   Lith_UnlockBIPPage(&p->bip, "Plasma");   break;
   case weapon_bfg:      Lith_UnlockBIPPage(&p->bip, "Cannon");   break;
   }
   
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
   
   if(ACS_PlayerNumber() == -1)
      return;
   
   ACS_GiveInventory("Lith_ScoreCount", amount);
   Lith_UpdateScore();
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
int Lith_GetPlayerData(int info, bool target)
{
   if(target)
      ACS_SetActivatorToTarget(0);
   
   if(ACS_PlayerNumber() < 0)
      return 0;
   
   player_t *p = &players[ACS_PlayerNumber()];
   
   switch(info)
   {
   case pdata_rifle_firemode: return p->riflefiremode;
   case pdata_shotgun_gauss:  return p->upgrades[UPGR_GaussShotty].active;
   case pdata_rocket_unreal:  return p->upgrades[UPGR_ChargeRPG].active;
   case pdata_plasma_laser:   return p->upgrades[UPGR_PlasLaser].active;
   case pdata_buttons:        return p->buttons;
   case pdata_has_sigil:      return p->sigil.acquired;
   case pdata_EXPLOOOOOSIONS: return p->upgrades[UPGR_TorgueMode].active;
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
   ACS_SetActivatorToTarget(0);
   
   player_t *p = &players[ACS_PlayerNumber()];
   
   if(!(p->weapons & (1 << user_pickupparm)))
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
int Lith_GetSigil()
{
   ACS_SetResultValue(1); // q_q
   
   player_t *p = &players[ACS_PlayerNumber()];
   
   if(p->cbi.open)
      Lith_KeyOpenCBI();
   
   p->sigil.acquired = true;
   
   if(ACS_GetCVar("__lith_debug_on"))
      return 1;
   
   p->frozen++;
   ACS_GiveInventory("Lith_TimeHax", 1);
   ACS_GiveInventory("Lith_TimeHax2", 1);
   
   ACS_SetActorVelocity(0, 0.0, 0.0, 0.0, false, true);
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

// EOF

