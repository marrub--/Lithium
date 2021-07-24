/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Adrenaline upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

dynam_aut script static
void DoAdrenaline() {
   pl.logH(4, tmpstr(lang(sl_log_adrenalineused)));
   StartSound(ss_player_adren_inj, lch_body2, CHANF_NOPAUSE|CHANF_MAYBE_LOCAL|CHANF_UI, 1.0, ATTN_STATIC);
   FadeFlash(255, 255, 255, 0.4, 0.6);
   FreezeTime(true);

   ACS_Delay(44);

   UnfreezeTime(true);
}

script
void Upgr_Adrenaline_Update(struct upgrade *upgr) {
   if(!has_status(so_StatAdrenalineCharging) &&
      !has_status(so_StatAdrenalineCharged) &&
      !has_status(so_StatAdrenaline))
   {
      add_status(so_StatAdrenalineCharging);
   }

   if(has_status(so_StatAdrenalineCharged) && ServCallI(sm_AdrenalineCheck)) {
      DoAdrenaline();
      add_status(so_StatAdrenaline);
   }
}

/* EOF */
