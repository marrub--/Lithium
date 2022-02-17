// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Handles payout information.                                              │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "w_world.h"

void Scr_HInit(void) {
   k64 taxpct = ACS_RandomFixed(0, 4 / 100.0);

#define GenPay(name, mul) \
   statement(if(payout.name##max) { \
      payout.name##pct = (payout.name##num / (k64)payout.name##max) * 100; \
      payout.name##scr = payout.name##pct * mul; \
   })

   GenPay(kill, 500);
   GenPay(item, 400);
   GenPay(scrt, 600);

#undef GenPay

   if(ticks <= payout.par)    payout.activities.par     = 10000;
   if(payout.killpct >= 100)  payout.activities.kill100 = 10000;
   if(payout.itempct >= 100)  payout.activities.item100 = 5000;
   if(payout.scrtpct >= 100)  payout.activities.scrt100 = 15000;
   if(ACS_Random(0, 10) == 0) payout.activities.sponsor = ACS_Random(1, 3) * 5000;

   payout.total  = payout.killscr + payout.itemscr;
   payout.total -= payout.tax = payout.total * taxpct;

   for(i32 i = 0, *p = (i32 *)&payout.activities;
       i < sizeof(payout.activities) / sizeof(i32);
       i++, p++) {
      payout.total += *p;
   }

   P_Scr_Payout();

   fastmemset(&payout, 0, sizeof payout);
}

/* EOF */
