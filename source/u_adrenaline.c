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

#define UData pl.upgrdata.adrenaline
#define CHARGE_MAX (35 * 30)

/* Static Functions -------------------------------------------------------- */

dynam_aut script static
void DoAdrenaline(struct upgrade *upgr) {
   UData.charge = UData.readied = 0;

   pl.logH(4, tmpstr(language(sl_log_adrenalineused)));
   StartSound(ss_player_adren_inj, lch_body2, CHANF_NOPAUSE|CHANF_MAYBE_LOCAL|CHANF_UI, 1.0, ATTN_STATIC);
   FadeFlash(255, 255, 255, 0.4, 0.6);
   FreezeTime(true);

   ACS_Delay(44);

   UnfreezeTime(true);
}

/* Extern Functions -------------------------------------------------------- */

script
void Upgr_Adrenaline_Update(struct upgrade *upgr)
{
   if(UData.charge < CHARGE_MAX) {
      /* Charge */
      UData.charge++;
   } else if(!UData.readied) {
      /* Prepare */
      StartSound(ss_player_adren_ready, lch_body2, CHANF_NOPAUSE|CHANF_MAYBE_LOCAL|CHANF_UI, 1.0, ATTN_STATIC);
      pl.logH(1, tmpstr(language(sl_log_adrenalineready)));
      UData.readied = true;
   } else if(ServCallI(sm_AdrenalineCheck)) {
      /* Ready to use */
      DoAdrenaline(upgr);
   }
}

void Upgr_Adrenaline_Render(struct upgrade *upgr)
{
   if(!get_bit(pl.upgrades[UPGR_HeadsUpDisM].flags, _ug_active)) return;

   k64 amt = UData.charge / (k64)CHARGE_MAX;

   str gfx = UData.readied ? sp_HUD_H_D24 : sp_HUD_H_D21;

   for(i32 i = 0; i < 20; i++) {
      i32 timemod = (pl.ticks - i) % 46;
      PrintSpriteA(gfx, 77-timemod,1, 224,1, (20 - i) / 20.0);
   }
}

/* EOF */
