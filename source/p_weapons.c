/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Weapon information handling.
 *
 * ---------------------------------------------------------------------------|
 */

#include "p_player.h"
#include "w_monster.h"
#include "w_world.h"
#include "p_hudid.h"

/* Static Functions -------------------------------------------------------- */

static
void GiveWeaponItem(i32 parm, i32 slot)
{
   switch(parm) {
   case weapon_c_fist:
   case weapon_fist:      ServCallI(sm_DieMonster);     break;
   case weapon_c_spas:    InvGive(so_ShellAmmo,  8);    break;
   case weapon_ssg:       InvGive(so_ShellAmmo,  4);    break;
   case weapon_c_sniper:  InvGive(so_RocketAmmo, 6);    break;
   case weapon_launcher:  InvGive(so_RocketAmmo, 2);    break;
   case weapon_c_plasma:
   case weapon_plasma:    InvGive(so_PlasmaAmmo, 1500); break;
   case weapon_c_shipgun: InvGive(so_CannonAmmo, 5);    break;
   case weapon_bfg:       InvGive(so_CannonAmmo, 4);    break;
   }
}

static
void WeaponGrab(struct weaponinfo const *info)
{
   if(!get_bit(pl.upgrades[UPGR_Seven7s].flags, _ug_active))
      ACS_LocalAmbientSound(info->pickupsound,  127);
   else
      ACS_LocalAmbientSound(ss_marathon_pickup, 127);

   switch(info->slot) {
   default: FadeFlash(255, 255, 255, 0.5, 0.4); break;
   case 3:  FadeFlash(0,   255, 0,   0.5, 0.5); break;
   case 4:  FadeFlash(255, 255, 0,   0.5, 0.5); break;
   case 5:  FadeFlash(255, 64,  0,   0.5, 0.6); break;
   case 6:  FadeFlash(0,   0,   255, 0.5, 0.6); break;
   case 7:  FadeFlash(255, 0,   0,   0.5, 0.7); break;
   }
}

static
void PickupScore(i32 parm)
{
   struct weaponinfo const *info = &weaponinfo[parm];

   i96 score = 4000 * info->slot;

   GiveWeaponItem(parm, info->slot);
   score = P_Scr_Give(score);

   P_Log_SellWeapon(info, score);
}

/* Extern Functions -------------------------------------------------------- */

void Wep_GInit(void)
{
   for(i32 i = 0; i < weapon_max; i++)
   {
      struct weaponinfo *info = (struct weaponinfo *)&weaponinfo[i];
      info->type = i;
   }
}

/* Update information on what weapons we have. */
script
void P_Wep_PTickPre()
{
   struct weapondata *w = &pl.weapon;

   w->prev = w->cur;

   /* Reset data temporarily. */
   w->cur = nil;
   for(i32 i = 0; i < SLOT_MAX; i++) w->slot[i] = 0;

   /* Iterate over each weapon setting information on it. */
   for(i32 i = weapon_min; i < weapon_max; i++)
   {
      struct weaponinfo const *info = &weaponinfo[i];
      struct invweapon *wep = &w->inv[i];

      if(!(pl.pclass & info->pclass) || !(wep->owned = InvNum(info->classname)))
         continue;

      w->slot[info->slot] += wep->owned;

      /* Check for currently held weapon. */
      if(!w->cur && pl.weaponclass == info->classname)
         w->cur = wep;

      wep->info      = info;
      wep->ammotype  = info->defammotype;
      wep->ammoclass = info->defammoclass;

      /* Special exceptions. */
      switch(i) {
      case weapon_shotgun:
         if(get_bit(pl.upgrades[UPGR_Shotgun_A].flags, _ug_active))
            wep->ammotype = AT_NMag;
         break;
      case weapon_c_spas:
         if(get_bit(pl.upgrades[UPGR_SPAS_B].flags, _ug_active))
            wep->ammotype = AT_Ammo;
         break;
      case weapon_plasma:
         if(get_bit(pl.upgrades[UPGR_Plasma_B].flags, _ug_active))
            wep->ammotype = AT_AMag;
         break;
      }

      switch(i)
      {
      /* For slot 3 weapons that don't take ammo, check if they should. */
      case weapon_shotgun:
      case weapon_c_rifle:
         if(CVarGetI(sc_weapons_slot3ammo)) {
            wep->ammotype |= AT_Ammo;
            wep->ammoclass = so_ShellAmmo;
         }
      }

      /* Set magazine and ammo counts. */
      if(w->cur == wep)
      {
         if(wep->ammotype & AT_NMag)
         {
            wep->magmax = ServCallI(sm_GetMaxMag, pl.num, wep->info->classname);
            wep->magcur = ServCallI(sm_GetCurMag, pl.num, wep->info->classname);
         }

         if(wep->ammotype & AT_Ammo)
         {
            wep->ammomax = InvMax(wep->ammoclass);
            wep->ammocur = InvNum(wep->ammoclass);
         }
      }

      /* Auto-reload. */
      if(pl.autoreload && wep->ammotype & AT_NMag && !(wep->ammotype & AT_Mana))
      {
         if(wep->autoreload >= 35 * 3)
            ServCallI(sm_AutoReload, pl.num, info->classname);

         if(w->cur != wep) wep->autoreload++;
         else              wep->autoreload = 0;
      }
   }

   if(!w->cur) w->cur = &w->inv[weapon_unknown];
}

script
void P_Wep_PTick()
{
   if(!Paused) {
      i32 heat = InvNum(so_SMGHeat);
           if(heat < 100) InvTake(so_SMGHeat, 5);
      else if(heat < 200) InvTake(so_SMGHeat, 4);
      else if(heat < 300) InvTake(so_SMGHeat, 3);
      else if(heat < 400) InvTake(so_SMGHeat, 2);
      else                InvTake(so_SMGHeat, 1);
   }

   if(pl.pclass == pcl_cybermage) {
                                     InvGive(so_Blade,    1);
                                     InvGive(so_Delear,   1);
      if(cbiupgr[cupg_c_slot3spell]) InvGive(so_Feuer,    1);
      if(cbiupgr[cupg_c_slot4spell]) InvGive(so_Rend,     1);
      if(cbiupgr[cupg_c_slot5spell]) InvGive(so_Hulgyon,  1);
      if(cbiupgr[cupg_c_slot6spell]) InvGive(so_StarShot, 1);
      if(cbiupgr[cupg_c_slot7spell]) InvGive(so_Cercle,   1);
   }

   SetSize(320, 240);

   switch(P_Wep_CurType())
   {
   case weapon_c_fist:
      Str(sl_mana_charge, sLANG "MANA_CHARGE");
      PrintTextA_str(L(sl_mana_charge), sf_smallfnt, CR_BRICK, 160,0, 100,0, 0.5);
      break;
   case weapon_c_delear:
      ServCallI(sm_DelearSprite);
      break;
   case weapon_cfist:
      __with(k64 charge = 5 + InvNum(so_FistCharge) / 10.0lk;)
      {
         PrintTextFmt(LC(LANG "CHARGE_FMT"), charge);
         PrintText(sf_smallfnt, CR_LIGHTBLUE, 270,2, 200,2);
      }
      break;
   }
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "WeaponPickup")
bool Sc_WeaponPickup(i32 name)
{
   if(P_None()) return false;

   Str(sc_sv_weaponstay, s"sc_sv_weaponstay");
   bool weaponstay = CVarGetI(sc_sv_weaponstay);
   i32 parm = weapon_unknown;

   parm = P_Wep_FromName(name);

   if(parm >= weapon_max || parm < weapon_min)
      return true;

   struct weaponinfo const *info = &weaponinfo[parm];

   if(HasWeapon(parm))
   {
      if(!weaponstay) {
         WeaponGrab(info);
         PickupScore(parm);
      }

      return !weaponstay;
   }
   else
   {
      WeaponGrab(info);

      pl.weaponsheld++;
      P_BIP_Unlock(info->name);

      GiveWeaponItem(parm, info->slot);
      P_Log_Weapon(info);
      InvGive(info->classname, 1);

      return !weaponstay;
   }
}

script_str ext("ACS") addr(OBJ "CircleSpread")
k32 Sc_CircleSpread(k32 mdx, k32 mdy, bool getpitch)
{
   noinit static
   k32 y, p;

   if(!getpitch)
   {
      k32 dx = ACS_RandomFixed(mdx,  0.0);
      k32 dy = ACS_RandomFixed(mdy,  0.0);
      k32 a  = ACS_RandomFixed(1.0, -1.0);

      y = ACS_Sin(a) * dx;
      p = ACS_Cos(a) * dy;

      return y;
   }
   else
      return p;
}

script_str ext("ACS") addr(OBJ "ChargeFistDamage")
i32 Sc_ChargeFistDamage(void)
{
   i32 amount = InvNum(so_FistCharge);
   InvTake(so_FistCharge, INT32_MAX);
   return amount * ACS_Random(1, 3);
}

script_str ext("ACS") addr(OBJ "AmmoRunOut")
k32 Sc_AmmoRunOut(bool ro, k32 mul)
{
   if(!P_None())
   {
      struct invweapon const *wep = pl.weapon.cur;
      k32 inv = wep->magcur / (k32)wep->magmax;

      mul = mul ? mul : 1.2;

      if(ro) inv = inv * mul;
      else   inv = mul - inv * 0.4;

           if(inv < 0) inv = 0;
      else if(inv > 1) inv = 1;

      return inv;
   }

   return 0;
}

script_str ext("ACS") addr(OBJ "GetFinalizerMaxHealth")
i32 Sc_GetFinalizerMaxHealth(void)
{
   i32 sh = ServCallI(sm_GetSpawnHealth);

   ifauto(dmon_t *, m, DmonSelf())
      return sh + (m->maxhealth - sh) * 0.5;
   else
      return sh;
}

alloc_aut(0) script_str ext("ACS") addr(OBJ "SurgeOfDestiny")
void Sc_SurgeOfDestiny(void)
{
   for(i32 i = 0; i < (35 * 17) / 2; i++) {
      ServCallI(sm_SurgeOfDestiny);
      ACS_Delay(2);
   }
}

script_str ext("ACS") addr(OBJ "GetWRF")
i32 Sc_GetWRF(void)
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

   i32 flags = 0;

   if(!P_None())
   {
      if(pl.semifrozen)
         flags |= WRF_NOFIRE;

      if(pl.pclass & (pcl_marine | pcl_darklord))
         flags |= WRF_ALLOWUSER4;
   }

   return flags;
}

alloc_aut(0) script_str ext("ACS") addr(OBJ "PoisonFXTicker")
void Sc_PoisonFXTicker(void) {
   for(i32 i = 0; i < 17; i++) {
      PausableTick();

      if(InvNum(so_PoisonFXReset)) {
         InvTake(so_PoisonFXReset, INT32_MAX);
         InvTake(so_PoisonFXTimer, INT32_MAX);
         ServCallI(sm_GivePoison);
         return;
      }
   }

   if(GetHealth(0) <= 0) {
      InvTake(so_PoisonFXReset, INT32_MAX);
      InvTake(so_PoisonFXTimer, INT32_MAX);
   } else if(InvNum(so_PoisonFXTimer)) {
      ServCallI(sm_PoisonFX);
      InvTake(so_PoisonFXTimer, 1);
   }
}

script_str ext("ACS") addr(OBJ "RecoilUp")
void Sc_RecoilUp(k32 amount)
{
   if(!P_None()) pl.extrpitch += amount / 180.0lk;
}

/* EOF */
