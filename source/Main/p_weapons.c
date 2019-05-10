/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Weapon information handling.
 *
 * ---------------------------------------------------------------------------|
 */

#if LITHIUM
#include "p_player.h"
#include "w_monster.h"
#include "w_world.h"
#include "p_hudid.h"

/* Static Functions -------------------------------------------------------- */

static void GiveWeaponItem(i32 parm, i32 slot)
{
   switch(parm) {
   case weapon_c_fist:
   case weapon_fist:      InvGive(so_Death,      1);    break;
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

static void WeaponGrab(struct player *p, struct weaponinfo const *info)
{
   if(!p->getUpgrActive(UPGR_7777777)) ACS_LocalAmbientSound(info->pickupsound,  127);
   else                                ACS_LocalAmbientSound(ss_marathon_pickup, 127);

   switch(info->slot) {
   default: FadeFlash(255, 255, 255, 0.5, 0.4); break;
   case 3:  FadeFlash(0,   255, 0,   0.5, 0.5); break;
   case 4:  FadeFlash(255, 255, 0,   0.5, 0.5); break;
   case 5:  FadeFlash(255, 64,  0,   0.5, 0.6); break;
   case 6:  FadeFlash(0,   0,   255, 0.5, 0.6); break;
   case 7:  FadeFlash(255, 0,   0,   0.5, 0.7); break;
   }
}

static void PickupScore(struct player *p, i32 parm)
{
   extern void P_Log_SellWeapon(struct player *p, struct weaponinfo const *info, i96 score);

   struct weaponinfo const *info = &weaponinfo[parm];

   i96 score = 4000 * info->slot;

   GiveWeaponItem(parm, info->slot);
   score = P_Scr_Give(p, score);

   P_Log_SellWeapon(p, info, score);
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
void P_Wep_PTickPre(struct player *p)
{
   struct weapondata *w = &p->weapon;

   w->prev = w->cur;

   /* Reset data temporarily. */
   w->cur = nil;
   for(i32 i = 0; i < SLOT_MAX; i++) w->slot[i] = 0;

   /* Iterate over each weapon setting information on it. */
   for(i32 i = weapon_min; i < weapon_max; i++)
   {
      struct weaponinfo const *info = &weaponinfo[i];
      struct invweapon *wep = &w->inv[i];

      if(!(p->pclass & info->pclass) || !(wep->owned = InvNum(info->classname)))
         continue;

      w->slot[info->slot] += wep->owned;

      /* Check for currently held weapon. */
      if(!w->cur && p->weaponclass == info->classname)
         w->cur = wep;

      wep->info      = info;
      wep->ammotype  = info->defammotype;
      wep->ammoclass = info->defammoclass;

      /* Special exceptions. */
      switch(i) {
      case weapon_shotgun: if(p->getUpgrActive(UPGR_GaussShotty)) wep->ammotype = AT_NMag; break;
      case weapon_c_spas:  if(p->getUpgrActive(UPGR_SPAS_B))      wep->ammotype = AT_Ammo; break;
      case weapon_plasma:  if(p->getUpgrActive(UPGR_PartBeam))    wep->ammotype = AT_AMag; break;
      }

      switch(i)
      {
      /* For slot 3 weapons that don't take ammo, check if they should. */
      case weapon_shotgun:
      case weapon_c_rifle:
         if(p->getCVarI(sc_weapons_slot3ammo)) {
            wep->ammotype |= AT_Ammo;
            wep->ammoclass = so_ShellAmmo;
         }
      }

      /* Set magazine and ammo counts. */
      if(w->cur == wep)
      {
         if(wep->ammotype & AT_NMag)
         {
            wep->magmax = ServCallI(sm_GetMaxMag, p->num, wep->info->classname);
            wep->magcur = ServCallI(sm_GetCurMag, p->num, wep->info->classname);
         }

         if(wep->ammotype & AT_Ammo)
         {
            wep->ammomax = InvMax(wep->ammoclass);
            wep->ammocur = InvNum(wep->ammoclass);
         }
      }

      /* Auto-reload. */
      if(p->autoreload && wep->ammotype & AT_NMag && !(info->flags & wf_magic))
      {
         if(wep->autoreload >= 35 * 3)
            ServCallI(sm_AutoReload, p->num, info->classname);

         if(w->cur != wep) wep->autoreload++;
         else              wep->autoreload = 0;
      }
   }

   if(!w->cur) w->cur = &w->inv[weapon_unknown];
}

script
void P_Wep_PTick(struct player *p)
{
   if(!Paused) {
      i32 heat = InvNum(so_SMGHeat);
           if(heat < 100) InvTake(so_SMGHeat, 5);
      else if(heat < 200) InvTake(so_SMGHeat, 4);
      else if(heat < 300) InvTake(so_SMGHeat, 3);
      else if(heat < 400) InvTake(so_SMGHeat, 2);
      else                InvTake(so_SMGHeat, 1);
   }

   if(p->pclass == pcl_cybermage) {
                                     InvGive(so_Blade,    1);
                                     InvGive(so_Delear,   1);
      if(cbiupgr[cupg_c_slot3spell]) InvGive(so_Feuer,    1);
      if(cbiupgr[cupg_c_slot4spell]) InvGive(so_Rend,     1);
      if(cbiupgr[cupg_c_slot5spell]) InvGive(so_Hulgyon,  1);
      if(cbiupgr[cupg_c_slot6spell]) InvGive(so_StarShot, 1);
      if(cbiupgr[cupg_c_slot7spell]) InvGive(so_Cercle,   1);
   }

   SetSize(320, 240);

   switch(P_Wep_CurType(p))
   {
   case weapon_c_fist:
      PrintTextA_str(L(st_mana_charge), s_cbifont, CR_BRICK, 160,0, 100,0, 0.5);
      break;
   case weapon_c_delear:
      ServCallI(sm_DelearSprite);
      break;
   case weapon_cfist:
      __with(k64 charge = 5 + InvNum(so_FistCharge) / 10.lk;)
      {
         PrintTextFmt(LC(LANG "CHARGE_FMT"), charge);
         PrintText(s_cbifont, CR_LIGHTBLUE, 270,2, 200,2);
      }
      break;
   }
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr("Lith_WeaponPickup")
bool Sc_WeaponPickup(i32 name)
{
   extern void P_Log_Weapon(struct player *p, struct weaponinfo const *info);
   extern i32 P_Wep_FromName(struct player *p, i32 name);

   struct player *p = LocalPlayer;
   if(P_None(p)) return false;

   bool weaponstay = ACS_GetCVar(sc_sv_weaponstay);
   i32 parm = weapon_unknown;

   parm = P_Wep_FromName(p, name);

   if(parm >= weapon_max || parm < weapon_min)
      return true;

   struct weaponinfo const *info = &weaponinfo[parm];

   if(HasWeapon(p, parm))
   {
      if(!weaponstay) {
         WeaponGrab(p, info);
         PickupScore(p, parm);
      }

      return !weaponstay;
   }
   else
   {
      WeaponGrab(p, info);

      p->weaponsheld++;
      bip_name_t tag; lstrcpy_str(tag, info->name);
      P_BIP_Unlock(p, tag);

      GiveWeaponItem(parm, info->slot);
      P_Log_Weapon(p, info);
      InvGive(info->classname, 1);

      return !weaponstay;
   }
}

script_str ext("ACS") addr("Lith_CircleSpread")
k32 Sc_CircleSpread(k32 mdx, k32 mdy, bool getpitch)
{
   static k32 A;
   static k32 P;

   if(!getpitch)
   {
      k32 dx = ACS_RandomFixed(mdx,  0.0);
      k32 dy = ACS_RandomFixed(mdy,  0.0);
      k32 a  = ACS_RandomFixed(1.0, -1.0);

      A = ACS_Sin(a) * dx;
      P = ACS_Cos(a) * dy;

      return A;
   }
   else
      return P;
}

script_str ext("ACS") addr("Lith_ChargeFistDamage")
i32 Sc_ChargeFistDamage(void)
{
   i32 amount = InvNum(so_FistCharge);
   InvTake(so_FistCharge, INT_MAX);
   return amount * ACS_Random(1, 3);
}

script_str ext("ACS") addr("Lith_AmmoRunOut")
k32 Sc_AmmoRunOut(bool ro, k32 mul)
{
   with_player(LocalPlayer)
   {
      struct invweapon const *wep = p->weapon.cur;
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

script_str ext("ACS") addr("Lith_GetFinalizerMaxHealth")
i32 Sc_GetFinalizerMaxHealth(void)
{
   i32 sh = GetPropI(0, APROP_SpawnHealth);

   ifauto(dmon_t *, m, DmonSelf())
      return sh + (m->maxhealth - sh) * 0.5;
   else
      return sh;
}

script_str ext("ACS") addr("Lith_SurgeOfDestiny")
void Sc_SurgeOfDestiny(void)
{
   for(i32 i = 0; i < (35 * 7) / 2; i++) {
      InvGive(so_SurgeOfDestiny, 1);
      ACS_Delay(2);
   }
}

script_str ext("ACS") addr("Lith_GetWRF")
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

   with_player(LocalPlayer)
   {
      if(p->semifrozen)
         flags |= WRF_NOFIRE;

      if(p->pclass & (pcl_marine | pcl_darklord))
         flags |= WRF_ALLOWUSER4;
   }

   return flags;
}

script_str ext("ACS") addr("Lith_PoisonFXTicker")
void Sc_PoisonFXTicker(void)
{
   for(i32 i = 0; i < 17; i++)
   {
      PausableTick();

      if(InvNum(so_PoisonFXReset))
      {
         InvTake(so_PoisonFXReset, INT_MAX);
         InvTake(so_PoisonFXTimer, INT_MAX);
         ServCallI(sm_GivePoison);
         return;
      }
   }

   if(GetPropI(0, APROP_Health) <= 0)
   {
      InvTake(so_PoisonFXReset, INT_MAX);
      InvTake(so_PoisonFXTimer, INT_MAX);
   }
   else if(InvNum(so_PoisonFXTimer))
   {
      ServCallI(sm_PoisonFX);
      InvTake(so_PoisonFXTimer, 1);
   }
}

script_str ext("ACS") addr("Lith_RecoilUp")
void Sc_RecoilUp(k32 amount)
{
   with_player(LocalPlayer) p->extrpitch += amount / 180.lk;
}
#endif

/* EOF */
