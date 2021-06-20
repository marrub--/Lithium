/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Timing information and setting.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "w_world.h"

/* Static Objects ---------------------------------------------------------- */

static
i32 lmvar frozen;

/* Extern Functions -------------------------------------------------------- */

cstr CanonTime(i32 type, u64 time) {
   noinit static
   char ft[64], st[64], dt[64], lf[256];

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
      case ct_full:  sprintf(ft, LanguageVC(lf, LANG "TIME_FMT_LONG"),  h, m, s, d, M, Y); return ft;
      case ct_short: sprintf(st, LanguageVC(lf, LANG "TIME_FMT_SHORT"), h, m,    d, M, Y); return st;
      case ct_date:  sprintf(dt, LanguageVC(lf, LANG "TIME_FMT_DATE"),           d, M, Y); return dt;
   }

   return nil;
}

void FreezeTime(bool players_ok) {
   frozen++;

   bool players_ok_mask = !singleplayer;

   if(!players_ok) pl.frozen++;
   if(!pl.frozen)  players_ok_mask = true;

   ServCallI(sm_SetFrozen, true, players_ok_mask);
}

void UnfreezeTime(bool players_ok) {
   if(frozen < 1) return;

   frozen--;
   if(!players_ok) pl.frozen--;

   if(frozen < 1) ServCallI(sm_SetFrozen, false, 0);
}

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "Timer")
i32 Sc_Timer(void) {
   return ACS_Timer();
}

/* EOF */
