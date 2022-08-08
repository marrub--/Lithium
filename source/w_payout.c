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

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

void Scr_MInit(void) {
   k64 taxpct = ACS_RandomFixed(0, 4 / 100.0);

#define GenPay(name, mul) \
   statement(if(wl.pay.name##max) { \
      wl.pay.name##pct = (wl.pay.name##num / (k64)wl.pay.name##max) * 100; \
      wl.pay.name##scr = wl.pay.name##pct * mul; \
   })

   GenPay(kill, 500);
   GenPay(item, 400);
   GenPay(scrt, 600);

#undef GenPay

   if(ACS_Timer() <= wl.pay.par) wl.pay.activities.par     = 10000;
   if(wl.pay.killpct >= 100)     wl.pay.activities.kill100 = 10000;
   if(wl.pay.itempct >= 100)     wl.pay.activities.item100 = 5000;
   if(wl.pay.scrtpct >= 100)     wl.pay.activities.scrt100 = 15000;
   if(ACS_Random(0, 10) == 0)    wl.pay.activities.sponsor = ACS_Random(1, 3) * 5000;

   wl.pay.total  = wl.pay.killscr + wl.pay.itemscr;
   wl.pay.total -= wl.pay.tax = wl.pay.total * taxpct;

   for(i32 i = 0, *p = (i32 *)&wl.pay.activities;
       i < sizeof(wl.pay.activities) / sizeof(i32);
       i++, p++) {
      wl.pay.total += *p;
   }

   P_Scr_Payout();

   fastmemset(&wl.pay, 0, sizeof wl.pay);
}

/* EOF */
