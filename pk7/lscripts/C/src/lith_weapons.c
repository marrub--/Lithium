#include "lith_common.h"
#include "lith_player.h"
#include "lith_pickups.h"

#include <math.h>

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
void Lith_PunctuatorFire(void)
{
   player_t *p = Lith_LocalPlayer;
   int ptid = ACS_UniqueTID();
   
   ACS_LineAttack(0, p->yaw, p->pitch, 128, "Lith_PunctuatorPuff", "None", 2048.0, FHF_NORANDOMPUFFZ, ptid);
   
   if(ACS_ThingCount(T_NONE, ptid))
   {
      fixed x = ACS_GetActorX(ptid);
      fixed y = ACS_GetActorY(ptid);
      fixed z = ACS_GetActorZ(ptid);
      
      float yaw = atan2f(y - p->y, x - p->x);
      
      float cx = cos(yaw);
      float cy = sin(yaw);
      
      for(int i = 0; i < 10; i++)
      {
         float sx = x + (cx * (32 * i));
         float sy = y + (cy * (32 * i));
         
         int etid = ACS_UniqueTID();
         
         ACS_SpawnForced("Lith_PunctuatorExplosion", sx, sy, z, etid);
         
         ACS_SetActivator(etid);
         ACS_SetPointer(AAPTR_TARGET, p->tid);
         ACS_SetActivator(p->tid);
      }
   }
}

// EOF

