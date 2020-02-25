/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
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
      ACS_PlaySound(0, ss_player_adren_ready, 5|CHAN_NOPAUSE|CHAN_MAYBE_LOCAL|CHAN_UI, 1.0, false, ATTN_STATIC);
      p->logH(1, "Adrenaline injector ready."); /* TODO */
      UData.readied = true;
   }

   /* Ready to use */
   else if(ServCallI(sm_AdrenalineCheck))
   {
      UData.charge = UData.readied = 0;

      p->logH(4, "Adrenaline administered."); /* TODO */
      ACS_PlaySound(0, ss_player_adren_inj, 5|CHAN_NOPAUSE|CHAN_MAYBE_LOCAL|CHAN_UI, 1.0, false, ATTN_STATIC);
      FadeFlash(255, 255, 255, 0.4, 0.6);
      InvGive(so_TimeHax2, 1);

      ACS_Delay(36);

      InvTake(so_TimeHax2, 1);
   }
}

stkcall
void Upgr_Adrenaline_Render(struct player *p, struct upgrade *upgr)
{
   if(!p->getUpgrActive(UPGR_HeadsUpDisM)) return;

   i32 timemod = p->ticks % 45;
   k64 amt = UData.charge / (k64)CHARGE_MAX;

   DrawSpriteXX(UData.readied ? sp_HUD_H_D24 : sp_HUD_H_D21,
      HUDMSG_FADEOUT | HUDMSG_ALPHA,
      hid_adrenind_fxS - timemod,
      77.1 - timemod,
      224.1,
      (k32)(0.3 * amt),
      (k32)(0.6 * amt),
      0.8);
}

/* EOF */
