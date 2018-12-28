// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "u_common.h"

#define UData UData_Adrenaline(upgr)
#define CHARGE_MAX (30 * 35)

// Extern Functions ----------------------------------------------------------|

stkcall
void Upgr_Adrenaline_Activate(struct player *p, upgrade_t *upgr)
{
   InvTake(so_AdrenalineToken, 1);
}

script
void Upgr_Adrenaline_Update(struct player *p, upgrade_t *upgr)
{
   // Charge
   if(UData.charge < CHARGE_MAX)
      UData.charge++;

   // Prepare
   else if(!UData.readied)
   {
      ACS_PlaySound(0, ss_player_adren_ready, 5|CHAN_NOPAUSE|CHAN_MAYBE_LOCAL|CHAN_UI, 1.0, false, ATTN_STATIC);
      p->logH(1, "Adrenaline injector ready."); // TODO
      UData.readied = true;
   }

   // Ready to use
   else
   {
      // Ready
      if(!InvNum(so_AdrenalineToken))
         InvGive(so_AdrenalineProjectileChecker, 1);

      // Use
      if(InvNum(so_AdrenalineToken))
      {
         InvTake(so_AdrenalineToken, 1);

         ACS_PlaySound(0, ss_player_adren_inj, 5|CHAN_NOPAUSE|CHAN_MAYBE_LOCAL|CHAN_UI, 1.0, false, ATTN_STATIC);
         p->logH(4, "Adrenaline administered."); // TODO

         UData.charge = UData.readied = 0;

         InvGive(so_TimeHax2, 1);
         ACS_Delay(36);
         InvTake(so_TimeHax2, 1);
      }
   }

   InvTake(so_AdrenalineToken, 1);
}

stkcall
void Upgr_Adrenaline_Render(struct player *p, upgrade_t *upgr)
{
   if(!p->getUpgrActive(UPGR_HeadsUpDisp)) return;

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

// EOF

