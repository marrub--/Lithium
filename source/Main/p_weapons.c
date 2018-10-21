// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
// vim: columns=110
#include "lith_player.h"
#include "lith_monster.h"
#include "lith_hudid.h"

// Static Functions ----------------------------------------------------------|

static void GiveWeaponItem(int parm, int slot)
{
   switch(parm)
   {
   case weapon_c_fist:
   case weapon_fist:      InvGive("Lith_Death",      1);    break;
   case weapon_c_spas:    InvGive("Lith_ShellAmmo",  8);    break;
   case weapon_ssg:       InvGive("Lith_ShellAmmo",  4);    break;
   case weapon_c_sniper:  InvGive("Lith_RocketAmmo", 6);    break;
   case weapon_launcher:  InvGive("Lith_RocketAmmo", 2);    break;
   case weapon_c_plasma:
   case weapon_plasma:    InvGive("Lith_PlasmaAmmo", 1500); break;
   case weapon_c_shipgun: InvGive("Lith_CannonAmmo", 5);    break;
   case weapon_bfg:       InvGive("Lith_CannonAmmo", 4);    break;
   }
}

static void WeaponGrab(struct player *p, weaponinfo_t const *info)
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

static void Lith_PickupScore(struct player *p, int parm)
{
   extern void Lith_SellMessage(struct player *p, weaponinfo_t const *info, i96 score);

   weaponinfo_t const *info = &weaponinfo[parm];

   i96 score = 4000 * info->slot;

   GiveWeaponItem(parm, info->slot);
   score = p->giveScore(score);

   Lith_SellMessage(p, info, score);
}

// Extern Functions ----------------------------------------------------------|

stkcall
int Lith_PlayerCurWeaponType(struct player *p)
{
   return p->weapon.cur->info->type;
}

script ext("ACS")
bool Lith_WeaponPickup(int name)
{
   extern void Lith_PickupMessage(struct player *p, weaponinfo_t const *info);
   extern int Lith_WeaponFromName(struct player *p, int name);

   struct player *p = LocalPlayer;
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
      InvGive(info->classname, 1);

      return !weaponstay;
   }
}

script ext("ACS")
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

script ext("ACS")
int Lith_ChargeFistDamage()
{
   int amount = InvNum("Lith_FistCharge");
   InvTake("Lith_FistCharge", 0x7FFFFFFF);
   return amount * ACS_Random(1, 3);
}

void Lith_GSInit_Weapon(void)
{
   for(int i = 0; i < weapon_max; i++)
   {
      weaponinfo_t *info = (weaponinfo_t *)&weaponinfo[i];
      info->type = i;
   }
}

// Update information on what weapons we have.
script
void Lith_PlayerPreWeapons(struct player *p)
{
   weapondata_t *w = &p->weapon;

   w->prev = w->cur;

   // Reset data temporarily.
   w->cur = null;
   for(int i = 0; i < SLOT_MAX; i++) w->slot[i] = 0;

   // Iterate over each weapon setting information on it.
   for(int i = weapon_min; i < weapon_max; i++)
   {
      weaponinfo_t const *info = &weaponinfo[i];
      invweapon_t *wep = &w->inv[i];

      if(!(p->pclass & info->pclass) || !(wep->owned = InvNum(info->classname)))
         continue;

      w->slot[info->slot] += wep->owned;

      // Check for currently held weapon.
      if(!w->cur && p->weaponclass == info->classname)
         w->cur = wep;

      wep->info      = info;
      wep->ammotype  = info->defammotype;
      wep->ammoclass = info->defammoclass;

      // Special exceptions.
      switch(i)
      {
      case weapon_shotgun: if(p->getUpgrActive(UPGR_GaussShotty)) wep->ammotype = AT_NMag; break;
      case weapon_c_spas:  if(p->getUpgrActive(UPGR_SPAS_B))      wep->ammotype = AT_Ammo; break;
      case weapon_plasma:  if(p->getUpgrActive(UPGR_PartBeam))    wep->ammotype = AT_AMag; break;
      }

      switch(i)
      {
      // For slot 3 weapons that don't take ammo, check if they should.
      case weapon_shotgun:
      case weapon_c_rifle:
         if(p->getCVarI("lith_weapons_slot3ammo")) {
            wep->ammotype |= AT_Ammo;
            wep->ammoclass = "Lith_ShellAmmo";
         }
      }

      // Set magazine and ammo counts.
      if(w->cur == wep)
      {
         if(wep->ammotype & AT_NMag)
         {
            wep->magmax = ServCallI("GetMaxMag", p->num, wep->info->classname);
            wep->magcur = ServCallI("GetCurMag", p->num, wep->info->classname);
         }

         if(wep->ammotype & AT_Ammo)
         {
            wep->ammomax = InvMax(wep->ammoclass);
            wep->ammocur = InvNum(wep->ammoclass);
         }
      }

      // Auto-reload.
      if(p->autoreload && wep->ammotype & AT_NMag && !(info->flags & wf_magic))
      {
         if(wep->autoreload >= 35 * 5)
            ServCallI("AutoReload", p->num, info->classname);

         if(w->cur != wep) wep->autoreload++;
         else              wep->autoreload = 0;
      }
   }

   if(!w->cur) w->cur = &w->inv[weapon_unknown];
}

script
void Lith_PlayerUpdateWeapons(struct player *p)
{
   if(!Lith_IsPaused)
   {
      int heat = InvNum("Lith_SMGHeat");
           if(heat < 100) InvTake("Lith_SMGHeat", 5);
      else if(heat < 200) InvTake("Lith_SMGHeat", 4);
      else if(heat < 300) InvTake("Lith_SMGHeat", 3);
      else if(heat < 400) InvTake("Lith_SMGHeat", 2);
      else                InvTake("Lith_SMGHeat", 1);
   }

   SetSize(320, 240);

   switch(p->weapontype)
   {
   case weapon_c_fist:
      PrintTextStr(L("LITH_MANA_CHARGE"));
      PrintTextA("cbifont", CR_BRICK, 160,0, 100,0, 0.5);
      break;
   case weapon_c_delear:
      ServCallI("Lith_DelearSprite");
      break;
   case weapon_cfist:
      __with(fixed64 charge = 5 + InvNum("Lith_FistCharge") / 10.lk;)
      {
         PrintTextFmt(L("LITH_CHARGE_FMT"), charge);
         PrintText("cbifont", CR_LIGHTBLUE, 270,2, 200,2);
      }
      break;
   }
}

script ext("ACS")
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

script ext("ACS")
int Lith_GetFinalizerMaxHealth(void)
{
   int sh = GetPropI(0, APROP_SpawnHealth);

   ifauto(dmon_t *, m, DmonSelf())
      return sh + (m->maxhealth - sh) * 0.5;
   else
      return sh;
}

script ext("ACS")
void Lith_SurgeOfDestiny(void)
{
   for(int i = 0; i < (35 * 7) / 2; i++) {
      InvGive("Lith_SurgeOfDestiny", 1);
      ACS_Delay(2);
   }
}

script ext("ACS")
int Lith_GetWRF(void)
{
   enum
   {
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

   int flags = 0;

   withplayer(LocalPlayer)
   {
      if(p->semifrozen)
         flags |= WRF_NOFIRE;

      if(p->pclass & (pcl_marine | pcl_darklord))
         flags |= WRF_ALLOWUSER4;
   }

   return flags;
}

script ext("ACS")
void Lith_PoisonFXTicker()
{
   for(int i = 0; i < 17; i++)
   {
      Lith_PausableTick();

      if(InvNum("Lith_PoisonFXReset"))
      {
         InvTake("Lith_PoisonFXReset", 999);
         InvTake("Lith_PoisonFXTimer", 999);
         InvGive("Lith_PoisonFXGiverGiver", 1);
         return;
      }
   }

   if(GetPropI(0, APROP_Health) <= 0)
   {
      InvTake("Lith_PoisonFXReset", 999);
      InvTake("Lith_PoisonFXTimer", 999);
      InvTake("Lith_PoisonFXGiverGiver", 999);
   }
   else if(InvNum("Lith_PoisonFXTimer"))
   {
      InvGive("Lith_PoisonFXGiver", 1);
      InvTake("Lith_PoisonFXTimer", 1);
   }
}

script ext("ACS")
void Lith_RecoilUp(fixed amount)
{
   withplayer(LocalPlayer) p->extrpitch += amount / 180.lk;
}

// EOF
