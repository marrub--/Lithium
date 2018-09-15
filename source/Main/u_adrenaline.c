// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

#define UData UData_Adrenaline(upgr)
#define CHARGE_MAX (30 * 35)

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
void Upgr_Adrenaline_Activate(struct player *p, upgrade_t *upgr)
{
   InvTake("Lith_AdrenalineToken", 1);
}

//
// Update
//
script
void Upgr_Adrenaline_Update(struct player *p, upgrade_t *upgr)
{
   // Charge
   if(UData.charge < CHARGE_MAX)
      UData.charge++;

   // Prepare
   else if(!UData.readied)
   {
      ACS_PlaySound(0, "player/adren/ready", 5|CHAN_NOPAUSE|CHAN_MAYBE_LOCAL|CHAN_UI, 1.0, false, ATTN_STATIC);
      p->logH(">>>>> Adrenaline injector ready.");
      UData.readied = true;
   }

   // Ready to use
   else
   {
      // Ready
      if(!InvNum("Lith_AdrenalineToken"))
         InvGive("Lith_AdrenalineProjectileChecker", 1);

      // Use
      if(InvNum("Lith_AdrenalineToken"))
      {
         InvTake("Lith_AdrenalineToken", 1);

         ACS_PlaySound(0, "player/adren/inj", 5|CHAN_NOPAUSE|CHAN_MAYBE_LOCAL|CHAN_UI, 1.0, false, ATTN_STATIC);
         p->logH(">>>>> Adrenaline administered.");

         UData.charge = UData.readied = 0;

         InvGive("Lith_TimeHax2", 1);
         ACS_Delay(36);
         InvTake("Lith_TimeHax2", 1);
      }
   }

   InvTake("Lith_AdrenalineToken", 1);
}

//
// Render
//
void Upgr_Adrenaline_Render(struct player *p, upgrade_t *upgr)
{
   if(!p->getUpgrActive(UPGR_HeadsUpDisp)) return;

   int timemod = p->ticks % 45;
   float amt = UData.charge / (float)CHARGE_MAX;

   DrawSpriteXX(UData.readied ? ":HUD:H_D24" : ":HUD:H_D21",
      HUDMSG_FADEOUT | HUDMSG_ALPHA,
      hid_adrenind_fxS - timemod,
      77.1 - timemod,
      224.1,
      (fixed)(0.3f * amt),
      (fixed)(0.6f * amt),
      0.8);
}

// EOF

