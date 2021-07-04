/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Subweapons upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

#define UData pl.upgrdata.subweapons

/* Extern Functions -------------------------------------------------------- */

script void Upgr_Subweapons_Update(struct upgrade *upgr) {
   if(UData.shots < 2) {
      if(UData.charge >= 100) {
         ACS_LocalAmbientSound(ss_weapons_subweapon_charge, 127);
         UData.shots++;
         UData.charge = 0;
      } else {
         UData.charge++;
      }
   }

   if(pl.buttons & BT_USER4 && !(pl.old.buttons & BT_USER4)) {
      for(u32 next = UData.which + 1;; next++) {
         if(next >= _subw_max) {
            next = 0;
         }

         if(get_bit(UData.have, next)) {
            UData.which = next;
            break;
         }
      }
   }
}

void Upgr_Subweapons_Render(struct upgrade *upgr) {
   if(!pl.hudenabled) return;

   PrintSprite(sp_SubWepBack, 66,1, 239,2);

   u32 prc = 47 * UData.charge / (k32)100.0;
   if(UData.shots == 0) SetClip(72, 223, prc, 1);
   /*                */ PrintSprite(sp_SubWepBar1, 72,1, 224,2);
   if(UData.shots == 1) SetClip(72, 223, prc, 1);
   if(UData.shots != 0) PrintSprite(sp_SubWepBar2, 72,1, 224,2);
   if(UData.shots <  2) ClearClip();

   for(i32 i = 0; i < _subw_max; i++) {
      i32 x   = 68 + i * 9;
      i32 fid = fid_subwepS + i;

      if(get_bit(UData.have, i)) PrintSprite(sa_subwep_act[i], x,1, 238,2);
      if(UData.which == i)       SetFade(fid, 1, 6);
      if(CheckFade(fid))         PrintSpriteF(sp_SubWepUse, x,1, 238,2, fid);
   }
}

void Upgr_Subweapons_Enter(struct ugprade *upgr) {
   UData.shots = 2;
   #ifndef NDEBUG
   if(get_bit(dbgflags, dbgf_items)) {
      UData.have = UINT32_MAX;
   } else {
   #endif
      set_bit(UData.have, _subw_gun);
   #ifndef NDEBUG
   }
   #endif
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "GetSubShots")
i32 Sc_GetSubShots(void) {
   if(!P_None()) {
      if(get_bit(pl.upgrades[UPGR_Subweapons].flags, _ug_active)) {
         return UData.shots;
      }
   }

   return 0;
}

script_str ext("ACS") addr(OBJ "TakeSubShot")
void Sc_TakeSubShot(void) {
   if(!P_None()) {
      if(get_bit(pl.upgrades[UPGR_Subweapons].flags, _ug_active)) {
         UData.shots--;
      }
   }
}

script_str ext("ACS") addr(OBJ "GetSubType")
i32 Sc_GetSubType(void) {
   if(!P_None()) {
      return UData.which;
   }
   return 0;
}

/* EOF */
