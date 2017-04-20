#include "lith_common.h"
#include "lith_player.h"

#include <math.h>

#define ValidateWeapon(parm) (parm < weapon_max && parm >= weapon_min)


//----------------------------------------------------------------------------
// Extern Objects
//

weaponinfo_t const weaponinfo[weapon_max] = {
// {S, pclass_type,      "Type-----------", "Pickup Sound-----------", AT_Type, "Ammo Class------------"},
   {0, pclass_any,       null,              "MMMMHMHMMMHMMM"},
   
   // Marine Weapons
   {1, pclass_marine,    "Fist",            "MMMMHMHMMMHMMM"},
   {1, pclass_marine,    "ChargeFist",      "weapons/cfist/pickup"},
   {2, pclass_marine,    "Pistol",          "weapons/pistol/pickup",    AT_Mag,  "Lith_PistolShotsFired"},
   {2, pclass_marine,    "Revolver",        "weapons/revolver/pickup",  AT_Mag,  "Lith_RevolverShotsFired"},
   {3, pclass_marine,    "Shotgun",         "weapons/shotgun/pickup"},
   {3, pclass_marine,    "LazShotgun",      "weapons/lshotgun/pickup"},
   {3, pclass_marine,    "SuperShotgun",    "weapons/ssg/pickup",       AT_Ammo, "Lith_ShellAmmo"},
   {4, pclass_marine,    "CombatRifle",     "weapons/rifle/pickup",     AT_Mag,  "Lith_RifleShotsFired"},
   {4, pclass_marine,    "SniperRifle",     "weapons/sniper/pickup",    AT_Mag,  "Lith_SniperShotsFired"},
   {5, pclass_marine,    "GrenadeLauncher", "weapons/rocket/pickup",    AT_Ammo, "Lith_RocketAmmo"},
   {5, pclass_marine,    "MissileLauncher", "weapons/missile/pickup",   AT_Ammo, "Lith_RocketAmmo"},
   {6, pclass_marine,    "PlasmaRifle",     "weapons/plasma/pickup",    AT_Ammo, "Lith_PlasmaAmmo"},
   {6, pclass_marine,    "PlasmaDiffuser",  "weapons/plasdiff/pickup",  AT_Ammo, "Lith_PlasmaAmmo"},
   {7, pclass_marine,    "BFG9000",         "weapons/cannon/pickup",    AT_Ammo, "Lith_CannonAmmo"},
   
   // Cyber-Mage Weapons
   {1, pclass_cybermage, "CFist",           "MMMMHMHMMMHMMM"},
   {2, pclass_cybermage, "Mateba",          "weapons/mateba/pickup",    AT_Mag,  "Lith_MatebaShotsFired"},
   {3, pclass_cybermage, "Delear",          "weapons/delear/pickup",    AT_Ammo, "Lith_DelearAmmo"},
   {5, pclass_cybermage, "IonRifle",        "weapons/ionrifle/pickup",  AT_Ammo, "Lith_RocketAmmo"},
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
   case weapon_ssg:      ACS_GiveInventory("Lith_ShellAmmo",  4);    break;
   case weapon_launcher: ACS_GiveInventory("Lith_RocketAmmo", 2);    break;
   case weapon_plasma:   ACS_GiveInventory("Lith_PlasmaAmmo", 1500); break;
   case weapon_bfg:      ACS_GiveInventory("Lith_CannonAmmo", 4);    break;
   }
}

//
// Lith_PickupScore
//
static void Lith_PickupScore(player_t *p, int parm)
{
   score_t score = 11100ll * weaponinfo[parm].slot;
   
   GiveWeaponItem(parm);
   p->giveScore(score);
   p->log("> Sold the %S for %lli\Cnscr\C-.",
      Language("LITH_TXT_INFO_SHORT_%S", weaponinfo[parm].name),
      p->getModScore(score));
}


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_WeaponPickup
//
[[__call("ScriptS"), __extern("ACS")]]
bool Lith_WeaponPickup(int name)
{
   extern void Lith_PickupMessage(player_t *p, weaponinfo_t const *info);
   
   bool weaponstay = ACS_GetCVar("sv_weaponstay");
   player_t *p = Lith_LocalPlayer;
   int parm = weapon_unknown;
   
   switch(p->pclass)
   {
   #define Case(name, set) case name: parm = set; break
   case pclass_marine:
      switch(name)
      {
      Case(wepnam_fist,           weapon_fist);
      Case(wepnam_chainsaw,       weapon_cfist);
      Case(wepnam_pistol,         weapon_pistol);
      Case(wepnam_shotgun,        weapon_shotgun);
      Case(wepnam_supershotgun,   weapon_ssg);
      Case(wepnam_chaingun,       weapon_rifle);
      Case(wepnam_rocketlauncher, weapon_launcher);
      Case(wepnam_plasmarifle,    weapon_plasma);
      Case(wepnam_bfg9000,        weapon_bfg);
      }
      break;
   
   case pclass_cybermage:
      switch(name)
      {
      Case(wepnam_fist,           weapon_c_fist);
      Case(wepnam_chainsaw,       weapon_cfist);
      Case(wepnam_pistol,         weapon_c_mateba);
      Case(wepnam_shotgun,        weapon_unknown);
      Case(wepnam_supershotgun,   weapon_unknown);
      Case(wepnam_chaingun,       weapon_unknown);
      Case(wepnam_rocketlauncher, weapon_c_sniper);
      Case(wepnam_plasmarifle,    weapon_unknown);
      Case(wepnam_bfg9000,        weapon_unknown);
      }
      break;
   #undef Case
   }
   
   if(!ValidateWeapon(parm))
      return true;
   
   if(HasWeapon(p, parm))
   {
      if(!weaponstay)
         Lith_PickupScore(p, parm);
      
      return !weaponstay;
   }
   
   weaponinfo_t const *info = &weaponinfo[parm];
   
   p->weaponsheld++;
   
   if(!p->getUpgr(UPGR_7777777)->active)
      ACS_LocalAmbientSound(info->pickupsound, 127);
   else
      ACS_LocalAmbientSound("marathon/pickup", 127);
   
   if(info->name) p->bip.unlock(info->name);
   
   GiveWeaponItem(parm);
   Lith_PickupMessage(p, info);
   
   ACS_GiveInventory(StrParam("Lith_%S", info->name), 1);
   
   return !weaponstay;
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
      fixed dx = ACS_RandomFixed(mdx, 0.0);
      fixed dy = ACS_RandomFixed(mdy, 0.0);
      fixed a  = ACS_RandomFixed(1.0, -1.0);
      
      A = sink(a) * dx;
      P = cosk(a) * dy;
      
      return bitsk(A);
   }
   else
      return bitsk(P);
}

//
// Lith_ChargeFistDamage
//
[[__call("ScriptS"), __extern("ACS")]]
int Lith_ChargeFistDamage()
{
   player_t *p = Lith_LocalPlayer;
   
   int amount = ACS_CheckInventory("Lith_FistCharge");
   ACS_TakeInventory("Lith_FistCharge", 0x7FFFFFFF);
   
   // We need to set the actor's TID even though we don't use it, because
   // some idiot thought it was a great idea to make PickActor return 0 when
   // their TID isn't set, even though it found something.
   int oldtid, newtid;
   if((oldtid = ACS_PickActor(0, p->yaw, p->pitch, 96, newtid = ACS_UniqueTID(), MF_SHOOTABLE, ML_BLOCKEVERYTHING | ML_BLOCKHITSCAN, PICKAF_RETURNTID)))
   {
      ACS_GiveInventory("Lith_FistHit", 1);
      ACS_Thing_ChangeTID(newtid, oldtid);
   }
   
   return amount * ACS_Random(1, 3);
}

//
// Lith_GSInit_Weapon
//
void Lith_GSInit_Weapon(void)
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
   
   // Reset data temporarily.
   w->cur = null;
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
      
      if(i == weapon_shotgun && p->getUpgr(UPGR_GaussShotty)->active)
      {
         wep->ammotype = AT_Mag;
         wep->ammoclass = "Lith_GaussShotsFired";
      }
      
      // Check for currently held weapon.
      if(!w->cur && ACS_StrICmp(p->weaponclass, info->class) == 0)
         w->cur = wep;
      
      if(p->getUpgr(UPGR_AutoReload)->active && wep->owned && wep->ammotype == AT_Mag)
      {
         if(wep->autoreload >= 35 * 5)
            ACS_TakeInventory(wep->ammoclass, 999);
         
         if(w->cur != wep) wep->autoreload++;
         else              wep->autoreload = 0;
      }
   }
   
   if(!w->cur) w->cur = &w->inv[weapon_unknown];
}

// EOF

