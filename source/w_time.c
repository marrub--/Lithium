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

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

static i32 lmvar frozen;

enum era {
   _era_bc,
   _era_ce,
   _era_ne,
};

enum weekday {
   _weekday_monday,
   _weekday_tuesday,
   _weekday_wednesday,
   _weekday_thursday,
   _weekday_friday,
   _weekday_saturday,
   _weekday_sunday,
   _weekday_max,
};

struct realtime {
   i32 s, m, h, d, M;
   time_t Y;
   enum era E;
   enum weekday D;
};

script cstr CanonTime(i32 type, time_t time) {
   static struct fmt_arg args[] = {
      {_fmt_i32, .precision = 2},
      {_fmt_i32, .precision = 2},
      {_fmt_i32, .precision = 2},
      {_fmt_i32}, {_fmt_i32}, {_fmt_i32},
      {_fmt_str}, {_fmt_str},
   };
   register timediv div;
   /* FIXME: make this use struct realtime */
   time_t s = time;     div = __div(s, 60); s = div.rem;
   time_t m = div.quot; div = __div(m, 60); m = div.rem;
   time_t h = div.quot; div = __div(h, 24); h = div.rem;
   time_t d = div.quot; div = __div(d, 30); d = div.rem;
   time_t M = div.quot; div = __div(M, 12); M = div.rem;
   time_t Y = div.quot;
   args[0].val.i = fastabs(h);
   args[1].val.i = fastabs(m);
   args[2].val.i = fastabs(s);
   args[3].val.i = fastabs(d);
   args[4].val.i = fastabs(M);
   args[5].val.i = Y;
   args[6].val.s = sl_time_era_ne;
   switch(fastabs(_weekday_friday + d) % _weekday_max) {
   case _weekday_monday:     args[7].val.s = sl_time_week_day_mon; break;
   case _weekday_tuesday:    args[7].val.s = sl_time_week_day_tue; break;
   case _weekday_wednesday:  args[7].val.s = sl_time_week_day_wed; break;
   case _weekday_thursday:   args[7].val.s = sl_time_week_day_thu; break;
   case _weekday_friday:     args[7].val.s = sl_time_week_day_fri; break;
   case _weekday_saturday:   args[7].val.s = sl_time_week_day_sat; break;
   case _weekday_sunday:     args[7].val.s = sl_time_week_day_sun; break;
   }
   ACS_BeginPrint();
   str fmt;
   switch(type) {
   case ct_full:  fmt = sl_time_fmt_long;  break;
   case ct_short: fmt = sl_time_fmt_short; break;
   case ct_date:  fmt = sl_time_fmt_date;  break;
   default: return nil;
   }
   printfmt(tmpstr(fmt), countof(args), args);
   return tmpstr(ACS_EndStrParam());
}

stkoff void FreezeTime(bool players_ok) {
   frozen++;
   if(!players_ok) pl.frozen++;
   ServCallV(sm_SetFrozen, true, !pl.frozen);
}

stkoff void UnfreezeTime(bool players_ok) {
   if(frozen < 1) return;
   frozen--;
   if(!players_ok) pl.frozen--;
   if(frozen < 1) ServCallV(sm_SetFrozen, false, 0);
}

void W_TickTime(void) {
   if(!Paused) {
      static time_t tick_sec;
      tick_sec += cv.sv_timescale;
      timediv d = __div(tick_sec, 35);
      wl.realtime += d.quot;
      tick_sec = d.rem;
   }
}

/* EOF */
