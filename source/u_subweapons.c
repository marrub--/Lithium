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
}

stkcall void Upgr_Subweapons_Render(struct player *p, struct upgrade *upgr) {
   if(!p->hudenabled || p->dlg.active) return;

   u32 prc = UData.charge ? UData.charge : 100;
   PrintTextFmt("SSHOTS %i\nCHARGE %03i%%", UData.shots, prc);
   PrintText(s_lmidfont, CR_WHITE, 10,1, 120,2);
}

stkcall void Upgr_Subweapons_Enter(struct player *p, struct ugprade *upgr) {
   UData.shots = 2;
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

/* EOF */
