#include "lith_common.h"
#include "lith_player.h"

#include <math.h>

#define ValidateWeapon(parm) (parm < weapon_max && parm >= weapon_min)


//----------------------------------------------------------------------------
// Extern Functions
//

void Lith_PickupMessage(player_t *p, weaponinfo_t const *info);


//----------------------------------------------------------------------------
// Extern Objects
//

weaponinfo_t const weaponinfo[weapon_max] = {
// {S, "Type-----------", "Pickup Sound-----------", AT_Type,  "Ammo Class------------"},
   {0, null,              "MMMMHMHMMMHMMM"},
   {1, "Fist",            "MMMMHMHMMMHMMM"},
   {2, "Pistol",          "weapons/pistol/pickup",   AT_Mag,  "Lith_PistolShotsFired"},
   {2, "Revolver",        "weapons/revolver/pickup", AT_Mag,  "Lith_RevolverShotsFired"},
   {3, "Shotgun",         "weapons/shotgun/pickup"},
   {3, "LazShotgun",      "weapons/lshotgun/pickup"},
   {4, "CombatRifle",     "weapons/rifle/pickup",    AT_Mag,  "Lith_RifleShotsFired"},
   {4, "SniperRifle",     "weapons/sniper/pickup",   AT_Mag,  "Lith_SniperShotsFired"},
   {5, "GrenadeLauncher", "weapons/rocket/pickup",   AT_Ammo, "Lith_RocketAmmo"},
   {5, "MissileLauncher", "weapons/missile/pickup",  AT_Ammo, "Lith_RocketAmmo"},
   {6, "PlasmaRifle",     "weapons/plasma/pickup",   AT_Ammo, "Lith_PlasmaAmmo"},
   {7, "BFG9000",         "weapons/cannon/pickup",   AT_Ammo, "Lith_CannonAmmo"},
};


//----------------------------------------------------------------------------
// Static Functions
//

//
// GiveWeaponItem
//
static void GiveWeaponItem(int parm)
{
   switch(parm)
   {
   case weapon_fist:     ACS_GiveInventory("Lith_Death",      1);    break;
   case weapon_launcher: ACS_GiveInventory("Lith_RocketAmmo", 10);   break;
   case weapon_plasma:   ACS_GiveInventory("Lith_PlasmaAmmo", 1500); break;
   case weapon_bfg:      ACS_GiveInventory("Lith_CannonAmmo", 4);    break;
   }
}


//----------------------------------------------------------------------------
// Scripts
//

//
// Lith_WeaponPickup
//
[[__call("ScriptI"), __address(14242), __extern("ACS")]]
void Lith_WeaponPickup(int parm, int tid)
{
   player_t *p = LocalPlayer;
   
   if(!ValidateWeapon(parm) || HasWeapon(p, parm))
      return;
   
   weaponinfo_t const *info = &weaponinfo[parm];
   
   p->weaponsheld++;
   
   if(!ACS_GetCVar("sv_weaponstay"))
      ACS_Thing_Remove(tid);
   
   if(!p->upgrades[UPGR_7777777].active)
      ACS_LocalAmbientSound(info->pickupsound, 127);
   else
      ACS_LocalAmbientSound("marathon/pickup", 127);
   
   if(info->name) Lith_UnlockBIPPage(&p->bip, info->name);
   
   GiveWeaponItem(parm);
   Lith_PickupMessage(p, info);
}

//
// Lith_PickupScore
//
[[__call("ScriptS"), __extern("ACS")]]
int Lith_PickupScore(int parm, int spritetid)
{
   ACS_SetActivatorToTarget(0);
   player_t *p = LocalPlayer;
   
   if(ACS_GetCVar("sv_weaponstay") || !ValidateWeapon(parm) || !HasWeapon(p, parm))
      return true;
   
   GiveWeaponItem(parm);
   Lith_Log(p, "> You sold the weapon for Score.");
   Lith_GiveScore(p, 11100 * parm);
   
   ACS_Thing_Remove(spritetid);
   
   return false;
}

//
// Lith_CircleSpread
//
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


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_SetupWeaponsTables
//
void Lith_SetupWeaponsTables(void)
{
   for(int i = 0; i < weapon_max; i++)
   {
      weaponinfo_t *info = (weaponinfo_t *)&weaponinfo[i];
      info->type  = i;
      info->class = StrParam("Lith_%S", info->name);
   }
}

//
// Lith_PlayerUpdateWeapon
//
// Update information on what weapons we have.
//
void Lith_PlayerUpdateWeapon(player_t *p)
{
   weapondata_t *w = &p->weapon;
   invweapon_t *unknown = &w->inv[weapon_unknown];
   
   // Reset data temporarily.
   w->cur = unknown;
   for(int i = 0; i < SLOT_MAX; i++)
      w->slot[i] = false;
   
   // Iterate over each weapon setting information on it.
   for(int i = weapon_min; i < weapon_max; i++)
   {
      weaponinfo_t const *info = &weaponinfo[i];
      invweapon_t *wep = &w->inv[i];
      
      w->slot[info->slot] |= wep->owned = ACS_CheckInventory(info->class);
      
      wep->info      = info;
      wep->owned     = ACS_CheckInventory(info->class);
      wep->ammotype  = info->defammotype;
      wep->ammoclass = info->defammoclass;
      
      if(i == weapon_shotgun && p->upgrades[UPGR_GaussShotty].active)
      {
         wep->ammotype = AT_Mag;
         wep->ammoclass = "Lith_GaussShotsFired";
      }
      
      // Check for currently held weapon.
      if(w->cur == unknown && ACS_StrICmp(p->weaponclass, info->class) == 0)
         w->cur = wep;
      
      if(p->upgrades[UPGR_AutoReload].active && wep->owned && wep->ammotype == AT_Mag)
      {
         if(wep->autoreload >= 35 * 5)
            ACS_TakeInventory(wep->ammoclass, 999);
         
         if(w->cur != wep) wep->autoreload++;
         else              wep->autoreload = 0;
      }
   }
}

// EOF

