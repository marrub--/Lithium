// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// w_payout.c: Handles payout information.

#if LITHIUM
#include "common.h"
#include "p_player.h"
#include "w_world.h"

void Lith_DoPayout(void)
{
   k64 taxpct = ACS_RandomFixed(0, 4 / 100.0);

   #define GenPay(name) \
      if(payout.name##max) \
      { \
         payout.name##pct = (payout.name##num / (k64)payout.name##max) * 100; \
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
#endif

// EOF
