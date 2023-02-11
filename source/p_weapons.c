// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Weapon information handling.                                             │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "p_player.h"
#include "w_monster.h"
#include "w_world.h"
#include "p_hudid.h"

static
void GiveWeaponItem(i32 parm, i32 slot) {
   switch(parm) {
   case weapon_m_shotgun: InvGive(so_ShellAmmo,  2);    break;
   case weapon_m_ssg:     InvGive(so_ShellAmmo,  4);    break;
   case weapon_m_rifle:   InvGive(so_BulletAmmo, 50);   break;
   case weapon_m_rocket:  InvGive(so_RocketAmmo, 2);    break;
   case weapon_m_plasma:  InvGive(so_PlasmaAmmo, 1500); break;
   case weapon_m_cannon:  InvGive(so_CannonAmmo, 4);    break;

   case weapon_c_erifle:   InvGive(so_BulletAmmo, 7);   break;
   case weapon_c_spas:     InvGive(so_ShellAmmo,  8);   break;
   case weapon_c_ionrifle: InvGive(so_RocketAmmo, 6);   break;
   case weapon_c_plasma:   InvGive(so_PlasmaAmmo, 750); break;
   case weapon_c_shipgun:  InvGive(so_CannonAmmo, 5);   break;

   case weapon_d_shrapnel: InvGive(so_ShellAmmo,  2);    break;
   case weapon_d_4bore:    InvGive(so_ShellAmmo,  16);   break;
   case weapon_d_minigun:  InvGive(so_BulletAmmo, 50);   break;
   case weapon_d_drocket:  InvGive(so_RocketAmmo, 4);    break;
   case weapon_d_fortune:  InvGive(so_PlasmaAmmo, 1750); break;
   }
}

static
void WeaponGrab(struct weaponinfo const *info) {
   AmbientSound(!get_bit(pl.upgrades[UPGR_Seven7s].flags, _ug_active) ?
                info->pickupsound :
                ss_marathon_pickup,
                1.0);

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
void PickupScore(i32 parm) {
   struct weaponinfo const *info = &weaponinfo[parm];

   score_t score = 4000 * info->slot;

   GiveWeaponItem(parm, info->slot);
   score = P_Scr_Give(GetX(0), GetY(0), GetZ(0), score, false);

   if(CVarGetI(sc_player_itemdisp) & _itm_disp_log) {
      P_Log_SellWeapon(parm, score);
   }
}

/* Update information on what weapons we have. */
script
void P_Wep_PTickPre(void) {
   struct weapondata *w = &pl.weapon;
   w->cur.type  = weapon_unknown;
   w->cur.info  = &weaponinfo[weapon_unknown];
   w->cur.class = fast_strupper(ACS_GetWeapon());
   for(i32 i = 0; i < SLOT_MAX; i++) {
      w->slot[i] = 0;
   }
   for(i32 i = weapon_min; i < weapon_max; i++) {
      struct weaponinfo const *info = &weaponinfo[i];
      if(!get_bit(info->pclass, pl.pclass)) {
         continue;
      }
      w->slot[info->slot] += InvNum(info->classname);
      if(w->cur.class != info->classname) {
         continue;
      }
      w->cur.type      = i;
      w->cur.info      = info;
      w->cur.ammotype  = info->defammotype;
      w->cur.ammoclass = info->defammoclass;
      switch(i) {
      case weapon_m_shotgun:
         if(get_bit(pl.upgrades[UPGR_Shotgun_A].flags, _ug_active)) {
            w->cur.ammotype = AT_InMa;
         }
         break;
      case weapon_m_plasma:
         if(get_bit(pl.upgrades[UPGR_Plasma_B].flags, _ug_active)) {
            w->cur.ammotype = AT_AmMa;
         }
         break;
      case weapon_c_spas:
         if(get_bit(pl.upgrades[UPGR_SPAS_B].flags, _ug_active)) {
            w->cur.ammotype = AT_AmIn;
         }
         break;
      case weapon_d_kampilan:
         if(get_bit(pl.upgrades[UPGR_Kampilan_A].flags, _ug_active)) {
            w->cur.ammotype = AT_Mgzn|AT_ADis;
         }
         break;
      }
      if(w->cur.ammotype & AT_Mgzn) {
         w->cur.magmax = ServCallI(sm_GetMaxMag, info->classname);
         w->cur.magcur = ServCallI(sm_GetCurMag, info->classname);
      }
      if(w->cur.ammotype & AT_Ammo) {
         w->cur.ammomax = InvMax(w->cur.ammoclass);
         w->cur.ammocur = InvNum(w->cur.ammoclass);
      }
   }
}

script
void P_Wep_PTick(void) {
   if(!Paused) {
      i32 heat = InvNum(so_SMGHeat);
           if(heat < 100) InvTake(so_SMGHeat, 5);
      else if(heat < 200) InvTake(so_SMGHeat, 4);
      else if(heat < 300) InvTake(so_SMGHeat, 3);
      else if(heat < 400) InvTake(so_SMGHeat, 2);
      else                InvTake(so_SMGHeat, 1);
   }

   SetSize(320, 240);

   switch(P_Wep_CurType()) {
   case weapon_c_fist:
      PrintTextA_str(ns(lang(sl_mana_charge)), sf_smallfnt, CR_BRICK, 160,0, 100,0, 0.5);
      break;
   case weapon_c_delear:
      ServCallV(sm_DelearSprite);
      break;
   case weapon_o_cfist: {
      i32 fcharg = InvNum(so_FistCharge);
      k64 charge = 5 + fcharg / 10.0lk;
      PrintTextFmt(tmpstr(lang(sl_charge_fmt)), charge, fcharg, fcharg * 3);
      PrintText(sf_smallfnt, CR_LIGHTBLUE, 270,3, 200,2);
      break;
   }
   }
}

script_str ext("ACS") addr(OBJ "WeaponPickup")
bool Z_WeaponPickup(i32 name) {
   bool weaponstay = CVarGetI(sc_sv_weaponstay);
   i32 parm = weapon_unknown;

   parm = Wep_FromName(name);

   if(parm >= weapon_max || parm < weapon_min) {
      return true;
   }

   struct weaponinfo const *info = &weaponinfo[parm];

   if(InvNum(info->classname)) {
      if(!weaponstay) {
         WeaponGrab(info);
         PickupScore(parm);
      }

      return !weaponstay;
   } else {
      WeaponGrab(info);

      pl.weaponsheld++;
      P_BIP_Unlock(P_BIP_NameToPage(info->name), false);

      GiveWeaponItem(parm, info->slot);
      P_Log_Weapon(parm);
      InvGive(info->classname, 1);

      return !weaponstay;
   }
}

script_str ext("ACS") addr(OBJ "ChargeFistDamage")
i32 Z_ChargeFistDamage(void) {
   i32 amount = InvNum(so_FistCharge);
   InvTake(so_FistCharge, INT32_MAX);
   return amount * ACS_Random(1, 3);
}

script_str ext("ACS") addr(OBJ "AmmoRunOut")
k32 Z_AmmoRunOut(bool ro, k32 mul) {
   struct invweapon const *wep = &pl.weapon.cur;
   k32 inv = wep->magcur / (k32)wep->magmax;

   mul = mul |? 1.2k;

   if(ro) inv = inv * mul;
   else   inv = mul - inv * 0.4;

   if(inv < 0) inv = 0;
   else if(inv > 1) inv = 1;

   return inv;
}

script_str ext("ACS") addr(OBJ "GetFinalizerMaxHealth")
i32 Z_GetFinalizerMaxHealth(void) {
   i32 sh = EDataI(_edt_spawnhealth);

   ifauto(dmon_t *, m, DmonSelf()) {
      return sh + (m->maxhealth - sh) * 0.5;
   } else {
      return sh;
   }
}

alloc_aut(0) script_str ext("ACS") addr(OBJ "SurgeOfDestiny")
void Z_SurgeOfDestiny(void) {
   for(i32 i = 0; i < (35 * 17) / 2; i++) {
      ServCallV(sm_SurgeOfDestiny);
      ACS_Delay(2);
   }
}

script_str ext("ACS") addr(OBJ "GetWRF")
i32 Z_GetWRF(void) {
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
   i32 flags = 0;
   if(pl.semifrozen) {
      flags |= WRF_NOFIRE;
   }
   if(get_bit(pM | pD, pl.pclass)) {
      flags |= WRF_ALLOWUSER4;
   }
   return flags;
}

script_str ext("ACS") addr(OBJ "RecoilUp")
void Z_RecoilUp(k32 amount) {
   pl.extrpitch += amount / 180.0lk;
}

/* EOF */
