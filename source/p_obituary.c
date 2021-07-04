/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Obituary string formatting and dispatch.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"

/* Scripts ----------------------------------------------------------------- */

script_str ext("ACS") addr(OBJ "Obituary")
void Sc_Obituary(void) {
   static
   struct {
      cstr sub, obj, psd, psi, act;
   } const pronoun[pro_max] = {
      {"they", "them", "their", "theirs", "they're"},
      {"she",  "her",  "her",   "hers",   "she's"  },
      {"he",   "him",  "his",   "his",    "he's"   },
      {"it",   "it",   "its",   "its'",   "it's"   },
   };

   str obit = ServCallS(sm_GetObituary);
   if(obit == st_nil) return;

   i32 rn = ACS_Random(1, 5);

   /**/ if(obit == st_ob_crush)    obit = lang_fmt(LANG "OB_Crush_%i",    rn);
   else if(obit == st_ob_default)  obit = lang_fmt(LANG "OB_Default_%i",  rn);
   else if(obit == st_ob_drowning) obit = lang_fmt(LANG "OB_Drowning_%i", rn);
   else if(obit == st_ob_exit)     obit = lang_fmt(LANG "OB_Exit_%i",     rn);
   else if(obit == st_ob_falling)  obit = lang_fmt(LANG "OB_Falling_%i",  rn);
   else if(obit == st_ob_fire)     obit = lang_fmt(LANG "OB_Fire_%i",     rn);
   else if(obit == st_ob_slime)    obit = lang_fmt(LANG "OB_Slime_%i",    rn);
   else if(obit == st_ob_suicide)  obit = lang_fmt(LANG "OB_Suicide_%i",  rn);

   noinit static
   char out[1024];

   char *pt = out;

   for(astr s = ns(obit); *s;) {
      cstr cs;
      str st;
      i32 ln;

      if(s[0] == '%') switch(s[1]) {
         case 'o': s += 2; st = pl.name;                 goto print_s;
         case 'g': s += 2; cs = pronoun[pl.pronoun].sub; goto print;
         case 'h': s += 2; cs = pronoun[pl.pronoun].obj; goto print;
         case 'p': s += 2; cs = pronoun[pl.pronoun].psd; goto print;
         case 's': s += 2; cs = pronoun[pl.pronoun].psi; goto print;
         case 'r': s += 2; cs = pronoun[pl.pronoun].act; goto print;
      print:   ln = faststrlen(cs);     faststrcpy(pt, cs); pt += ln; continue;
      print_s: ln = ACS_StrLen(st); faststrcpy_str(pt, st); pt += ln; continue;
      }

      *pt++ = *s++;
   }

   *pt = '\0';

   if(obit != st_nil) {
      Dbg_Log(log_dev, "%s", out);
      pl.logB(1, "%s", out);
   }
}

/* EOF */
