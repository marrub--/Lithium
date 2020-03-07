/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Handles payout information.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"

void Scr_HInit(void)
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

   for_player()
   {
      script extern void P_Scr_Payout(struct player *p);
      P_Scr_Payout(p);
   }

   fastmemset(&payout, 0, sizeof payout);
}

/* EOF */
