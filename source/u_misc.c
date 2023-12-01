// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Miscellaneous upgrade functions.                                         │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"

void Upgr_DefenseNuke_Enter(void) {
   ServCallV(sm_Nuke);
}

void Upgr_InstaDeath_Update(void) {
   if(pl.health < pl.old.health) {
      ServCallV(sm_DieNow);
   }
}

void Upgr_Goldeneye_Update(void) {
   if(ACS_Random(0, 0x7F) < 0x50) {
      k32 dist = ACS_RandomFixed(128, 256);
      k32 angl = ACS_RandomFixed(0, 1);
      ACS_SpawnForced(so_GEExplosion, pl.x + ACS_Cos(angl) * dist, pl.y + ACS_Sin(angl) * dist, pl.z + 32);
   }
}

noinit static str lolsords_origweapon;

void Upgr_lolsords_Activate(void) {
   lolsords_origweapon = pl.weapon.cur.class;
   InvGive(so_Sword, 1);
}

void Upgr_lolsords_Deactivate(void) {
   InvTake(so_Sword, 1);
   ACS_SetWeapon(lolsords_origweapon);
}

void Upgr_lolsords_Update(void) {
   ACS_SetWeapon(so_Sword);
}

static void player_fall_damage(k32 damagefac, k32 speedfac) {
   if(pl.velz == 0 && pl.old.velz < -16 * speedfac) {
      ServCallV(sm_FallDamage, fastabsk(pl.old.velz) * damagefac);
   }
}

void Upgr_CyberLegs_Update(void) {
   pl.speedmul  += 20;
   pl.jumpboost += 50;
   if(!pl.frozen) {
      player_fall_damage(1.0k, 1.0k);
   }
}

void Upgr_JetBooster_Update(void) {
   if(!pl.frozen && P_ButtonPressed(BT_JUMP) && !pl.onground &&
      (!get_bit(pl.upgrades[UPGR_ReflexWetw].flags, _ug_active) ||
       pl.upgrdata.reflexwetw.leaped == 2) &&
      !has_status(so_StatJetBooster) &&
      !has_status(so_StatJetLag)) {
      k32 angle = pl.yaw - atan2k(pl.sidev, pl.forwardv);
      add_status(so_StatJetBooster);
      StartSound(ss_player_lane_rocketboost, lch_auto, 0, 1.0k);
      P_SetVel(pl.velx + ACS_Cos(angle) * 16.0, pl.vely + ACS_Sin(angle) * 16.0, 10.0);
   }
}

void Upgr_HeavyArmor_Activate(void) {
   SetMass(0, 120);
   SetGravity(0, 1.1k);
   set_bit(pl.upgrdata.subweapons.have, _subw_fist);
}

void Upgr_HeavyArmor_Deactivate(void) {
   SetMass(0, 110);
   SetGravity(0, 1.05k);
   dis_bit(pl.upgrdata.subweapons.have, _subw_fist);
   if(pl.upgrdata.subweapons.which == _subw_fist) {
      pl.upgrdata.subweapons.which = 0;
   }
}

void Upgr_HeavyArmor_Update(void) {
   if(!pl.frozen) {
      player_fall_damage(1.5k, 0.8k);
   }
}

noinit static struct k32v2 hoverboots_origspeed;
noinit static i32 hoverboots_tics;
noinit static bool hoverboots_on;

void Upgr_HoverBoots_Update(void) {
   if(pl.frozen) {
      return;
   }
   if(!hoverboots_tics || pl.onground || !P_ButtonHeld(BT_JUMP)) {
      hoverboots_on = false;
      hoverboots_origspeed.x = pl.velx;
      hoverboots_origspeed.y = pl.vely;
      if(pl.onground) {
         hoverboots_tics = 60;
      }
   } else if(hoverboots_tics && pl.velz <= 0) {
      hoverboots_on = true;
   }
   if(hoverboots_on) {
      StartSound(ss_player_ari_float, lch_body2, CHANF_LOOP|CHANF_NOSTOP, 1.0k);
      k32 vx = hoverboots_origspeed.x;
      k32 vy = hoverboots_origspeed.y;
      k32 vz = 5;
      if(P_ButtonHeld(BT_SPEED) &&
         get_bit(pl.upgrades[UPGR_HoverBoots2].flags, _ug_active)) {
         k32 angle = pl.yaw - atan2k(pl.sidev, pl.forwardv);
         vx = ACS_Cos(angle) * 14;
         vy = ACS_Sin(angle) * 14;
         vz = 0;
      } else {
         vx += pl.sidev;
         vy += pl.forwardv;
      }
      P_SetVel(vx, vy, vz);
      --hoverboots_tics;
   } else {
      ACS_StopSound(0, lch_body2);
   }
}

void Upgr_SkillSword_Enter(void) {
   ServCallV(sm_KhandaPower);
   AmbientSound(ss_weapons_khanda_pwup, 0.25k);
   P_CenterNotification(sl_go_kill, 35*3, CR_RED, 0xFF0000, 1, 1);
   ACS_SetWeapon(so_Khanda);
}

/* EOF */
