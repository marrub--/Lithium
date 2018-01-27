// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
// vim: columns=110
#include "lith_player.h"
#include "lith_monster.h"
#include "lith_hudid.h"

#include <math.h>

// Static Functions ----------------------------------------------------------|

//
// GiveWeaponItem
//
static void GiveWeaponItem(int parm, int slot)
{
   switch(parm)
   {
   case weapon_c_fist:
   case weapon_fist:      ACS_GiveInventory("Lith_Death",      1);    break;
   case weapon_c_spas:    ACS_GiveInventory("Lith_ShellAmmo",  8);    break;
   case weapon_ssg:       ACS_GiveInventory("Lith_ShellAmmo",  4);    break;
   case weapon_c_sniper:  ACS_GiveInventory("Lith_RocketAmmo", 6);    break;
   case weapon_launcher:  ACS_GiveInventory("Lith_RocketAmmo", 2);    break;
   case weapon_c_plasma:
   case weapon_plasma:    ACS_GiveInventory("Lith_PlasmaAmmo", 1500); break;
   case weapon_c_shipgun: ACS_GiveInventory("Lith_CannonAmmo", 5);    break;
   case weapon_bfg:       ACS_GiveInventory("Lith_CannonAmmo", 4);    break;

   case weapon_fd_jpcp_chainsaw:
      ACS_GiveInventory("FDGotChainsaw", 1);
      break;
   }

   if(parm > weapon_max_lith) switch(slot)
   {
   case 2: ACS_GiveInventory("Lith_BulletAmmo", 20); break;
   case 3: ACS_GiveInventory("Lith_ShellAmmo",  10); break;
   case 4: ACS_GiveInventory("Lith_BulletAmmo", 40); break;
   case 5: ACS_GiveInventory("Lith_RocketAmmo", 5);  break;
   case 6: ACS_GiveInventory("Lith_PlasmaAmmo", 50); break;
   case 7: ACS_GiveInventory("Lith_CannonAmmo", 2);  break;
   }
}

//
// WeaponGrab
//
static void WeaponGrab(player_t *p, weaponinfo_t const *info)
{
   if(!p->getUpgrActive(UPGR_7777777)) ACS_LocalAmbientSound(info->pickupsound, 127);
   else                                ACS_LocalAmbientSound("marathon/pickup", 127);

   switch(info->slot)
   {
   default: Lith_FadeFlash(255, 255, 255, 0.5, 0.4); break;
   case 3:  Lith_FadeFlash(0,   255, 0,   0.5, 0.5); break;
   case 4:  Lith_FadeFlash(255, 255, 0,   0.5, 0.5); break;
   case 5:  Lith_FadeFlash(255, 64,  0,   0.5, 0.6); break;
   case 6:  Lith_FadeFlash(0,   0,   255, 0.5, 0.6); break;
   case 7:  Lith_FadeFlash(255, 0,   0,   0.5, 0.7); break;
   }
}

//
// Lith_PickupScore
//
static void Lith_PickupScore(player_t *p, int parm)
{
   weaponinfo_t const *info = &weaponinfo[parm];

   score_t score = 4000 * info->slot;

   GiveWeaponItem(parm, info->slot);
   score = p->giveScore(score);

   if(info->type < weapon_max_lith)
      p->log("> Sold the %S for %lli\Cnscr\C-.", Language("LITH_TXT_INFO_SHORT_%S", info->name), score);
   else
      p->log("> Sold the slot %i weapon for %lli\Cnscr\C-.", info->slot, score);
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_WeaponPickup
//
[[__call("ScriptS"), __extern("ACS")]]
bool Lith_WeaponPickup(int name)
{
   extern void Lith_PickupMessage(player_t *p, weaponinfo_t const *info);
   extern int Lith_WeaponFromName(player_t *p, int name);

   player_t *p = LocalPlayer;
   if(NoPlayer(p)) return false;

   bool weaponstay = ACS_GetCVar("sv_weaponstay");
   int parm = weapon_unknown;

   parm = Lith_WeaponFromName(p, name);

   if(parm >= weapon_max || parm < weapon_min)
      return true;

   weaponinfo_t const *info = &weaponinfo[parm];

   if(HasWeapon(p, parm))
   {
      if(!weaponstay) {
         WeaponGrab(p, info);
         Lith_PickupScore(p, parm);
      }

      return !weaponstay;
   }
   else
   {
      WeaponGrab(p, info);

      p->weaponsheld++;
      p->bipUnlock(info->name);

      GiveWeaponItem(parm, info->slot);
      Lith_PickupMessage(p, info);

      if(info->type != weapon_fd_jpcp_chainsaw) // fuck a bitch
         ACS_GiveInventory(info->classname, 1);

      return !weaponstay;
   }
}

//
// Lith_CircleSpread
//
[[__call("ScriptS"), __extern("ACS")]]
fixed Lith_CircleSpread(fixed mdx, fixed mdy, bool getpitch)
{
   static fixed A;
   static fixed P;

   if(!getpitch)
   {
      fixed dx = ACS_RandomFixed(mdx,  0.0);
      fixed dy = ACS_RandomFixed(mdy,  0.0);
      fixed a  = ACS_RandomFixed(1.0, -1.0);

      A = ACS_Sin(a) * dx;
      P = ACS_Cos(a) * dy;

      return A;
   }
   else
      return P;
}

//
// Lith_ChargeFistDamage
//
[[__call("ScriptS"), __extern("ACS")]]
int Lith_ChargeFistDamage()
{
   int amount = ACS_CheckInventory("Lith_FistCharge");
   ACS_TakeInventory("Lith_FistCharge", 0x7FFFFFFF);
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
      info->type = i;
   }
}

//
// Lith_PlayerPreWeapons
//
// Update information on what weapons we have.
//
void Lith_PlayerPreWeapons(player_t *p)
{
   weapondata_t *w = &p->weapon;

   w->prev = w->cur;

   // Reset data temporarily.
   w->cur = null;
   for(int i = 0; i < SLOT_MAX; i++)
      w->slot[i] = 0;

   // Iterate over each weapon setting information on it.
   for(int i = weapon_min; i < weapon_max; i++)
   {
      weaponinfo_t const *info = &weaponinfo[i];
      invweapon_t *wep = &w->inv[i];

      w->slot[info->slot] += (wep->owned = ACS_CheckInventory(info->classname));

      // Check for currently held weapon.
      if(!w->cur && p->weaponclass == info->classname)
         w->cur = wep;

      wep->info      = info;
      wep->owned     = ACS_CheckInventory(info->classname);
      wep->ammotype  = info->defammotype;
      wep->ammoclass = info->defammoclass;

      if(!(wep->ammotype & AT_ZScr))
         wep->magclass  = info->defmagclass;

      // Special exceptions.
      switch(i)
      {
      case weapon_shotgun: if(p->getUpgrActive(UPGR_GaussShotty)) wep->ammotype = AT_ZMag; break;
      case weapon_c_spas:  if(p->getUpgrActive(UPGR_SPAS_B))      wep->ammotype = AT_Ammo; break;
      }

      // Set magazine class if this weapon doesn't take ammo.
      if(wep->ammotype & AT_NMag && !(wep->ammotype & AT_Ammo))
         wep->magclass = wep->ammoclass;

      // For slot 3 weapons that don't take ammo, check if they should.
      switch(i)
      case weapon_shotgun:
      case weapon_c_rifle:
      case weapon_d_4bore:
         if(p->getCVarI("lith_weapons_slot3ammo")) {
            wep->ammotype |= AT_Ammo;
            wep->ammoclass = "Lith_ShellAmmo";
         }

      // Set magazine and ammo counts.
      if(w->cur == wep)
      {
         if(wep->ammotype & AT_NMag)
         {
            if(wep->ammotype & AT_ZScr)
            {
               wep->magmax = HERMES("GetMaxMag", p->num, wep->info->classname);
               wep->magcur = HERMES("GetCurMag", p->num, wep->info->classname);
            }
            else
            {
               wep->magmax = ACS_GetMaxInventory(0, wep->magclass);
               wep->magcur = ACS_CheckInventory (   wep->magclass);
            }
         }

         if(wep->ammotype & AT_Ammo)
         {
            wep->ammomax = ACS_GetMaxInventory(0, wep->ammoclass);
            wep->ammocur = ACS_CheckInventory (   wep->ammoclass);
         }
      }

      // Remove inactive magic weapons.
      else if(info->flags & wf_magic && wep->owned && ++wep->magictake > 20)
      {
         ACS_TakeInventory(info->classname, 1);
         wep->magictake = 0;
      }

      // Auto-reload anything else.
      if(p->getUpgrActive(UPGR_AutoReload) && wep->owned && wep->ammotype & AT_NMag &&
         !(info->flags & wf_magic))
      {
         if(wep->autoreload >= 35 * 5) {
            if(wep->ammotype & AT_ZScr) HERMES("AutoReload", p->num, info->classname);
            else                        ACS_TakeInventory(wep->magclass, 999);
         }

         if(w->cur != wep) wep->autoreload++;
         else              wep->autoreload = 0;
      }
   }

   if(!w->cur) w->cur = &w->inv[weapon_unknown];
}

//
// Lith_PlayerUpdateWeapons
//
[[__call("ScriptS")]]
void Lith_PlayerUpdateWeapons(player_t *p)
{
   __with(int heat = ACS_CheckInventory("Lith_SMGHeat");)
   {
           if(heat < 100) ACS_TakeInventory("Lith_SMGHeat", 5);
      else if(heat < 200) ACS_TakeInventory("Lith_SMGHeat", 4);
      else if(heat < 300) ACS_TakeInventory("Lith_SMGHeat", 3);
      else if(heat < 400) ACS_TakeInventory("Lith_SMGHeat", 2);
      else                ACS_TakeInventory("Lith_SMGHeat", 1);
   }

   switch(p->weapontype)
   {
   case weapon_c_delear: ACS_GiveInventory("Lith_DelearSpriteDisplay", 1); break;
   case weapon_cfist:
      ACS_SetHudSize(320, 240);
      fixed64_t charge = 5.lk + ACS_CheckInventory("Lith_FistCharge") / 10.lk;
      HudMessageF("CBIFONT", "\Cn%.1lkkV \Cjcharge", charge);
      HudMessagePlain(hid_fistcharge, 270.2, 200.2, TS);
      break;
   }
}

//
// Lith_AmmoRunOut
//
[[__call("ScriptS"), __extern("ACS")]]
fixed Lith_AmmoRunOut(bool ro, fixed mul)
{
   withplayer(LocalPlayer)
   {
      invweapon_t const *wep = p->weapon.cur;
      fixed inv = wep->magcur / (fixed)wep->magmax;

      mul = mul ? mul : 1.2;

      if(ro) inv = inv * mul;
      else   inv = mul - inv * 0.35;

      return minmax(inv, 0.0, 1.0);
   }

   return 0;
}

//
// Lith_GetFinalizerMaxHealth
//
[[__call("ScriptS"), __extern("ACS")]]
int Lith_GetFinalizerMaxHealth(void)
{
   int sh = ACS_GetActorProperty(0, APROP_SpawnHealth);

   ifauto(dmon_t *, m, DmonPtr())
      return sh + (m->maxhealth - sh) * 0.5;
   else
      return sh;
}

//
// Lith_SwitchRifleMode
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_SwitchRifleFiremode(void)
{
   withplayer(LocalPlayer)
   {
      int max = rifle_firemode_max;

      if(!p->getUpgrActive(UPGR_RifleModes))
         max--;

      p->riflefiremode = ++p->riflefiremode % max;
      ACS_LocalAmbientSound("weapons/rifle/firemode", 127);
   }
}

//
// Lith_ResetRifleMode
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_ResetRifleMode()
{
   withplayer(LocalPlayer)
      if(p->getCVarI("lith_weapons_riflemodeclear"))
         p->riflefiremode = rifle_firemode_auto;
}

//
// Lith_SurgeOfDestiny
//
[[__call("ScriptS"), __extern("ACS")]]
void Lith_SurgeOfDestiny(void)
{
   for(int i = 0; i < (35 * 7) / 2; i++) {
      ACS_GiveInventory("Lith_SurgeOfDestiny", 1);
      ACS_Delay(2);
   }
}

//
// Lith_GetWRF
//
[[__call("ScriptS"), __extern("ACS")]]
int Lith_GetWRF(void)
{
   enum {
      WRF_NOBOB         = 1,
      WRF_NOSWITCH      = 2,
      WRF_NOPRIMARY     = 4,
      WRF_NOSECONDARY   = 8,
      WRF_NOFIRE        = WRF_NOPRIMARY | WRF_NOSECONDARY,
      WRF_ALLOWRELOAD   = 16,
      WRF_ALLOWZOOM     = 32,
      WRF_DISABLESWITCH = 64,
      WRF_ALLOWUSER1    = 128,
      WRF_ALLOWUSER2    = 256,
      WRF_ALLOWUSER3    = 512,
      WRF_ALLOWUSER4    = 1024
   };

   withplayer(LocalPlayer) {
      int flags = 0;
      if(p->semifrozen)           flags |= WRF_NOFIRE;
      if(p->pclass == pcl_marine) flags |= WRF_ALLOWUSER4;
      return flags;
   }
   return 0;
}

// EOF

