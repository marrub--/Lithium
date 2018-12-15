// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"

StrEntOFF

void Lith_DoPayout(void)
{
   fixed64 taxpct = ACS_RandomFixed(0, 4 / 100.0);

   #define GenPay(name) \
      if(payout.name##max) \
      { \
         payout.name##pct = (payout.name##num / (fixed64)payout.name##max) * 100; \
         payout.name##scr = payout.name##pct * 600; \
      }

   GenPay(kill)
   GenPay(item)

   #undef GenPay

   payout.total  = payout.killscr + payout.itemscr;
   payout.total -= payout.tax = payout.total * taxpct;

   Lith_ForPlayer()
   {
      script
      extern void Lith_PlayerPayout(struct player *p);

      Lith_PlayerPayout(p);
   }

   memset(&payout, 0, sizeof payout);
}

// EOF
