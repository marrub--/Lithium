/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
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

script_str ext("ACS") addr("Lith_Obituary")
void Sc_Obituary(void)
{
   Str(ob_crush,      s"(crush)");
   Str(ob_default,    s"(default)");
   Str(ob_drowning,   s"(drowning)");
   Str(ob_exit,       s"(exit)");
   Str(ob_falling,    s"(falling)");
   Str(ob_fire,       s"(fire)");
   Str(ob_slime,      s"(slime)");
   Str(ob_suicide,    s"(suicide)");

   static cstr pronoun[pro_max][5] = {
      {"they", "them", "their", "theirs", "they're"},
      {"she",  "her",  "her",   "hers",   "she's"  },
      {"he",   "him",  "his",   "his",    "he's"   },
      {"it",   "it",   "its",   "its'",   "it's"   },
   };

   struct player *p = LocalPlayer;

   str obit = ServCallS(sm_GetObituary);
   if(obit == s_NIL) return;

   i32 rn = ACS_Random(1, 5);

   /**/ if(obit == ob_crush)    obit = Language(LANG "OB_Crush_%i",    rn);
   else if(obit == ob_default)  obit = Language(LANG "OB_Default_%i",  rn);
   else if(obit == ob_drowning) obit = Language(LANG "OB_Drowning_%i", rn);
   else if(obit == ob_exit)     obit = Language(LANG "OB_Exit_%i",     rn);
   else if(obit == ob_falling)  obit = Language(LANG "OB_Falling_%i",  rn);
   else if(obit == ob_fire)     obit = Language(LANG "OB_Fire_%i",     rn);
   else if(obit == ob_slime)    obit = Language(LANG "OB_Slime_%i",    rn);
   else if(obit == ob_suicide)  obit = Language(LANG "OB_Suicide_%i",  rn);

   noinit static char out[1024];
   char *pt = out;

   for(astr s = obit; *s;) {
      cstr cs;
      str st;
      i32 len;

      if(s[0] == '%') switch(s[1]) {
         case 'o': s += 2; st = p->name;                goto print_s;
         case 'g': s += 2; cs = pronoun[p->pronoun][0]; goto print;
         case 'h': s += 2; cs = pronoun[p->pronoun][1]; goto print;
         case 'p': s += 2; cs = pronoun[p->pronoun][2]; goto print;
         case 's': s += 2; cs = pronoun[p->pronoun][3]; goto print;
         case 'r': s += 2; cs = pronoun[p->pronoun][4]; goto print;
      print:
         len =     strlen(cs);      strcpy(pt, cs); pt += len; continue;
      print_s:
         len = ACS_StrLen(st); lstrcpy_str(pt, st); pt += len; continue;
      }

      *pt++ = *s++;
   }

   *pt = '\0';

   if(obit != s_NIL) {
      Dbg_Log(log_dev, "%s", out);
      for_player() p->logB(1, "%s", out);
   }
}

/* EOF */
