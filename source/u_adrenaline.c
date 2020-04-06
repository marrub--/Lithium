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

#define UData p->upgrdata.adrenaline
#define CHARGE_MAX (35 * 30)

/* Extern Functions -------------------------------------------------------- */

script
void Upgr_Adrenaline_Update(struct player *p, struct upgrade *upgr)
{
   /* Charge */
   if(UData.charge < CHARGE_MAX)
      UData.charge++;

   /* Prepare */
   else if(!UData.readied)
   {
      StartSound(ss_player_adren_ready, lch_body2, CHANF_NOPAUSE|CHANF_MAYBE_LOCAL|CHANF_UI, 1.0, ATTN_STATIC);
      p->logH(1, LC(LANG "LOG_AdrenalineReady"));
      UData.readied = true;
   }

   /* Ready to use */
   else if(ServCallI(sm_AdrenalineCheck))
   {
      UData.charge = UData.readied = 0;

      p->logH(4, LC(LANG "LOG_AdrenalineUsed"));
      StartSound(ss_player_adren_inj, lch_body2, CHANF_NOPAUSE|CHANF_MAYBE_LOCAL|CHANF_UI, 1.0, ATTN_STATIC);
      FadeFlash(255, 255, 255, 0.4, 0.6);
      InvGive(so_TimeHax2, 1);

      ACS_Delay(36);

      InvTake(so_TimeHax2, 1);
   }
}

stkcall
void Upgr_Adrenaline_Render(struct player *p, struct upgrade *upgr)
{
   if(!p->upgrades[UPGR_HeadsUpDisM].active) return;

   k64 amt = UData.charge / (k64)CHARGE_MAX;

   str gfx = UData.readied ? sp_HUD_H_D24 : sp_HUD_H_D21;

   for(i32 i = 0; i < 20; i++) {
      i32 timemod = (p->ticks - i) % 46;
      PrintSpriteA(gfx, 77-timemod,1, 224,1, (20 - i) / 20.0);
   }
}

/* EOF */
