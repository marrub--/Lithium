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

   if(!p->hudenabled) return;
   u32 prc = UData.charge ? UData.charge : 100;
   PrintTextChS("shots\ncharge\nwhich\ntemp UI");
   PrintText(s_lmidfont, CR_WHITE, 75,2, 120,1);
   PrintTextFmt("%i\n%03i%%\n%s", UData.shots, prc, LanguageC(LANG "INFO_SHORT_Subweapon%i", UData.which));
   PrintText(s_lmidfont, CR_WHITE, 80,1, 120,1);
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
