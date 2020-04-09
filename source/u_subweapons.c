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

#define UData p->upgrdata.subweapons

/* Extern Functions -------------------------------------------------------- */

script void Upgr_Subweapons_Update(struct player *p, struct upgrade *upgr) {
   if(UData.shots < 2) {
      if(UData.charge >= 100) {
         ACS_LocalAmbientSound(ss_weapons_subweapon_charge, 127);
         UData.shots++;
         UData.charge = 0;
      } else {
         UData.charge++;
      }
   }

   if(p->buttons & BT_USER4 && !(p->old.buttons & BT_USER4)) {
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

stkcall void Upgr_Subweapons_Render(struct player *p, struct upgrade *upgr) {
   Str(subwepback, s":HUD_D:SubWepBack");
   Str(subwepbar1, s":HUD_D:SubWepBar1");
   Str(subwepbar2, s":HUD_D:SubWepBar2");

   if(!p->hudenabled) return;

   PrintSprite(subwepback, 66,1, 239,2);

   u32 prc = 47 * UData.charge / (k32)100.0;
   if(UData.shots == 0) SetClip(72, 223, prc, 1);
   /*                */ PrintSprite(subwepbar1, 72,1, 224,2);
   if(UData.shots == 1) SetClip(72, 223, prc, 1);
   if(UData.shots != 0) PrintSprite(subwepbar2, 72,1, 224,2);
   if(UData.shots <  2) ClearClip();

   for(i32 i = 0; i < _subw_max; i++) {
      StrAry(subwepact,
             s":HUD_D:SubWep0", s":HUD_D:SubWep1", s":HUD_D:SubWep2",
             s":HUD_D:SubWep3", s":HUD_D:SubWep4", s":HUD_D:SubWep5");
      Str(subwepuse, s":HUD_D:SubWepUse");

      i32 x   = 68 + i * 9;
      i32 fid = fid_subwepS + i;

      if(get_bit(UData.have, i)) PrintSprite(subwepact[i], x,1, 238,2);
      if(UData.which == i)       SetFade(fid, 1, 6);
      if(CheckFade(fid))         PrintSpriteF(subwepuse, x,1, 238,2, fid);
   }
}

stkcall void Upgr_Subweapons_Enter(struct player *p, struct ugprade *upgr) {
   UData.shots = 2;
   if(get_bit(dbgflag, dbgf_items)) UData.have = UINT32_MAX;
   else                             set_bit(UData.have, _subw_gun);
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr("Lith_GetSubShots")
i32 Sc_GetSubShots(void) {
   with_player(LocalPlayer) {
      if(get_bit(p->upgrades[UPGR_Subweapons].flags, _ug_active)) {
         return UData.shots;
      }
   }

   return 0;
}

script_str ext("ACS") addr("Lith_TakeSubShot")
void Sc_TakeSubShot(void) {
   with_player(LocalPlayer) {
      if(get_bit(p->upgrades[UPGR_Subweapons].flags, _ug_active)) {
         UData.shots--;
      }
   }
}

script_str ext("ACS") addr("Lith_GetSubType")
i32 Sc_GetSubType(void) {
   with_player(LocalPlayer) {
      return UData.which;
   }
   return 0;
}

/* EOF */
