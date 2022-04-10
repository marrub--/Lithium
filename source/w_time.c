// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Timing information and setting.                                          │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "w_world.h"

static
i32 lmvar frozen;

cstr CanonTime(i32 type, i32 time) {
   noinit static
   char ft[64], st[64], dt[64], fmt[64];

   i32 t = time;
   i32 s = 53 + t / 35;
   i32 m = 30 + s / 60;
   i32 h = 14 + m / 60;
   i32 d = 25 + h / 24;
   i32 M =  6 + d / 30;
   i32 Y = 48 + M / 11;

   s %= 60;
   m %= 60;
   h %= 24;

   d = d % 31 + 1;
   M = M % 12 + 1;
   Y = Y      + 1;

   switch(type) {
   case ct_full:
      faststrcpy_str(fmt, ns(lang(sl_time_fmt_long)));
      sprintf(ft, fmt, h, m, s, d, M, Y);
      return ft;
   case ct_short:
      faststrcpy_str(fmt, ns(lang(sl_time_fmt_short)));
      sprintf(st, fmt, h, m, d, M, Y);
      return st;
   case ct_date:
      faststrcpy_str(fmt, ns(lang(sl_time_fmt_date)));
      sprintf(dt, fmt, d, M, Y);
      return dt;
   }

   return nil;
}

alloc_aut(0) stkcall
void FreezeTime(bool players_ok) {
   frozen++;
   if(!players_ok) pl.frozen++;
   ServCallV(sm_SetFrozen, true, !pl.frozen);
}

alloc_aut(0) stkcall
void UnfreezeTime(bool players_ok) {
   if(frozen < 1) return;
   frozen--;
   if(!players_ok) pl.frozen--;
   if(frozen < 1) ServCallV(sm_SetFrozen, false, 0);
}

script_str ext("ACS") addr(OBJ "Timer")
i32 Z_Timer(void) {
   return ACS_Timer();
}

/* EOF */
