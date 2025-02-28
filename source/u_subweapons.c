// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Subweapons upgrade.                                                      │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"

#define udata pl.upgrdata.subweapons

enum {
   _uptic_time = 2,
   _uptic_count_1 = countof(sa_subwep_up_1),
   _uptic_count_2 = countof(sa_subwep_up_2),
};

Decl_Update void Upgr_Subweapons_Update(void) {
   udata.charge_max = 120 - attr_refchargebuff();
   if(udata.uptics) {
      --udata.uptics;
   }
   if(udata.shots < 2) {
      if(udata.charge >= udata.charge_max) {
         udata.shots++;
         udata.charge = 0;
         StartSound(ss_weapons_subweapon_charge, lch_ambient, CHANF_NOPAUSE|CHANF_NOSTOP, 1.0k, ATTN_NONE, 0.75k + 0.25k * udata.shots);
         udata.uptics = _uptic_time * (udata.shots == 1 ? _uptic_count_1 : _uptic_count_2);
      } else {
         udata.charge++;
      }
   }
   if(P_ButtonPressed(BT_USER4)) {
      for(i32 next = udata.which + 1;; next++) {
         if(next >= _subw_max) {
            next = 0;
         }
         if(get_bit(udata.have, next)) {
            udata.which = next;
            break;
         }
      }
   }
}

Decl_Render void Upgr_Subweapons_Render(void) {
   if(!pl.hudenabled) return;
   if(!get_bit(udata.have, _subw_fist)) {
      PrintSprite(sp_SubWepBack, pl.hudhppos+1,1, 239,2);
   } else {
      PrintSprite(sp_SubWepBack2, pl.hudhppos+1,1, 239,2);
      PrintSprite(sp_SubWepFist, pl.hudhppos+39,1, 238,2);
   }
   PrintSprite(sp_SubWepFront, pl.hudhppos+3,1, 238,2);
   i32 prc = 29 * udata.charge / (k32)udata.charge_max;
   i32 srw;
   if(udata.shots == 0) {srw = prc;} else {srw = 29;}
   /*                */ PrintSpriteClip(sp_SubWepBar1, pl.hudhppos+7,1, 224,2, 0,0,srw,1);
   if(udata.shots == 1) {srw = prc;} else {srw = 29;}
   if(udata.shots >  0) PrintSpriteClip(sp_SubWepBar2, pl.hudhppos+7,1, 224,2, 0,0,srw,1);
   for(i32 i = 0; i < _subw_max; i++) {
      i32 x   = pl.hudhppos + 3 + i * 9;
      i32 fid = fid_subwepS + i;
      if(get_bit(udata.have, i)) PrintSprite(sa_subwep_act[i], x,1, 238,2);
      if(udata.which == i)       SetFade(fid, 1, 6);
      if(CheckFade(fid))         PrintSprite(sp_SubWepUse, x,1, 238,2, _u_fade, fid);
   }
   if(udata.uptics) {
      str *sp = udata.shots == 1 ? sa_subwep_up_1 : sa_subwep_up_2;
      i32 n = udata.uptics / _uptic_time;
      if(udata.shots != 1 || n != 5) {
         PrintSprite(sp[n], pl.hudhppos+36,0, 223,0);
      }
   }
}

Decl_Enter void Upgr_Subweapons_Enter(struct ugprade *upgr) {
   udata.shots = 2;
   #ifndef NDEBUG
   if(dbgflags(dbgf_items)) {
      udata.have |= UINT32_MAX & ~dst_bit(_subw_fist);
   } else {
   #endif
      set_bit(udata.have, _subw_gun);
   #ifndef NDEBUG
   }
   #endif
}

script_str alloc_aut(0) ext("ACS") addr(OBJ "GetSubShots")
i32 Z_GetSubShots(void) {
   return udata.shots;
}

script_str alloc_aut(0) ext("ACS") addr(OBJ "TakeSubShot")
void Z_TakeSubShot(void) {
   udata.shots--;
}

script_str alloc_aut(0) ext("ACS") addr(OBJ "GetSubType")
i32 Z_GetSubType(void) {
   return udata.which;
}

alloc_aut(0) script_str type("net") ext("ACS") addr(OBJ "KeySetSubweapon")
void Z_KeySetSubweapon(i32 which) {
   if(get_bit(udata.have, which)) {
      udata.which = which;
   }
}

/* EOF */
