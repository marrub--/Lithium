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

stkcall void rtime(time_t time, struct realtime *rt) {
   if(!rt) {
      return;
   }
   timediv tdiv = __div(time, DAYS(1));
   i32div idiv;
   idiv = __div((i32)tdiv.rem, 60); rt->s = idiv.rem;
   idiv = __div(idiv.quot,     60); rt->m = idiv.rem; rt->h = idiv.quot;
   tdiv = __div(tdiv.quot,     30); rt->d = tdiv.rem;
   tdiv = __div(tdiv.quot,     12); rt->M = tdiv.rem; rt->Y = tdiv.quot;
   rt->D = rt->d % _weekday_max;
   if(rt->Y < BEGINNING_OF_UNIVERSE) {
      rt->Y = BEGINNING_OF_UNIVERSE - rt->Y;
      rt->E = _era_bce;
   } else if(rt->Y < CALAMITY_EPOCH) {
      rt->Y = rt->Y - BEGINNING_OF_UNIVERSE;
      rt->E = _era_ce;
   } else {
      rt->Y = rt->Y - CALAMITY_EPOCH;
      rt->E = _era_ne;
   }
}

script cstr CanonTime(i32 type, time_t time) {
   static struct fmt_arg args[] = {
      {_fmt_i32, .precision = 2},
      {_fmt_i32, .precision = 2},
      {_fmt_i32, .precision = 2},
      {_fmt_i32}, {_fmt_i32}, {_fmt_time},
      {_fmt_str}, {_fmt_str},
   };
   struct realtime rt; rtime(time, &rt);
   args[0].val.i = rt.h;
   args[1].val.i = rt.m;
   args[2].val.i = rt.s;
   args[3].val.i = rt.d + 1;
   args[4].val.i = rt.M + 1;
   args[5].val.fmt_time_val = rt.Y;
   switch(rt.E) {
   case _era_bce: args[6].val.s = sl_time_era_bce; break;
   case _era_ce:  args[6].val.s = sl_time_era_ce;  break;
   case _era_ne:  args[6].val.s = sl_time_era_ne;  break;
   }
   switch(rt.D) {
   case _weekday_monday:     args[7].val.s = sl_time_week_day_mon; break;
   case _weekday_tuesday:    args[7].val.s = sl_time_week_day_tue; break;
   case _weekday_wednesday:  args[7].val.s = sl_time_week_day_wed; break;
   case _weekday_thursday:   args[7].val.s = sl_time_week_day_thu; break;
   case _weekday_friday:     args[7].val.s = sl_time_week_day_fri; break;
   case _weekday_saturday:   args[7].val.s = sl_time_week_day_sat; break;
   case _weekday_sunday:     args[7].val.s = sl_time_week_day_sun; break;
   }
   str fmt;
   switch(type) {
   case ct_full:  fmt = sl_time_fmt_long;  break;
   case ct_short: fmt = sl_time_fmt_short; break;
   case ct_date:  fmt = sl_time_fmt_date;  break;
   default: return nil;
   }
   ACS_BeginPrint();
   noinit static char buf[256];
   faststrcpy_str(buf, fmt);
   printfmt(buf, countof(args), args);
   faststrcpy_str(buf, ACS_EndStrParam());
   return buf;
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
      static i96 tick_sec;
      i64 timescale = ml.timescale;
      #ifndef NDEBUG
      if(dbgflags(dbgf_xtime)) {
         timescale *= (i64)cv.sv_timescale * fastabsl(cv.sv_timescale);
      } else {
      #endif
         timescale *= cv.sv_timescale;
      #ifndef NDEBUG
      }
      #endif
      tick_sec    += timescale;
      i96div d     = __div(tick_sec, 35);
      wl.realtime += d.quot;
      tick_sec     = d.rem;
   }
}

script_str ext("ACS") addr("Made in Heaven")
void Z_Certainty(void) {
   time_t origin = wl.realtime;
   time_t lasttime;
   bool rampdown = false;
   for(;;) {
      i64 newscale = ml.timescale + 60 + ml.timescale / 60;
      if(ml.timescale < 30 * 35 * 3) {
         ml.timescale += 3;
      } else if(newscale > ml.timescale) {
         ml.timescale = minli(newscale, YEARS(BEGINNING_OF_UNIVERSE) * 10);
      }
      ACS_Delay(1);
      if(lasttime > wl.realtime) {
         FadeFlash(0, 0, 0, 1.0k, 10.0k);
         ACS_BeginPrint();
         _p(st_heaven);
         ACS_EndPrint();
         wl.realtime = origin - 1000000;
         ml.timescale = 10000;
         break;
      }
      lasttime = wl.realtime;
   }
   for(i32 i = 0; i < 100; ++i) {
      ml.timescale -= 100;
      ACS_Delay(1);
   }
}

/* EOF */
