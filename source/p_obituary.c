// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Obituary string formatting and dispatch.                                 │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"

enum {
   _pn_sub,
   _pn_obj,
   _pn_psd,
   _pn_psi,
   _pn_act,
   _pn_max
};

typedef cstr pronoun_set[_pn_max];

script_str ext("ACS") addr(OBJ "Obituary")
void Sc_Obituary(void) {
   static
   pronoun_set const defaultpronoun[] = {
      {"he",   "him",  "his",   "his",    "he's"   },
      {"she",  "her",  "her",   "hers",   "she's"  },
      {"they", "them", "their", "theirs", "they're"},
      {"it",   "it",   "its",   "its'",   "it's"   },
   };

   noinit static
   char pbuf[64];

   noinit static
   pronoun_set set;

   ifw(str pset = CVarGetS(sc_player_pronouns), !pset[0]) {
      fastmemcpy(set, &defaultpronoun[ACS_GetPlayerInfo(ACS_PlayerNumber(),
                                                        PLAYERINFO_GENDER)],
                 sizeof(pronoun_set));
   } else {
      faststrcpy_str(pbuf, pset);

      i32 i = 0;
      for(char *next = nil,
               *word = faststrtok(pbuf, &next, '/');
          word && i < _pn_max;
          word = faststrtok(nil, &next, '/'))
      {
         set[i++] = word;
      }
   }

   str obit = ServCallS(sm_GetObituary);
   if(!obit[0]) return;

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

      if(*s == '%') {
         switch(*++s) {
         case 'o': st = pl.name;      goto print_s;
         case 'g': cs = set[_pn_sub]; goto print;
         case 'h': cs = set[_pn_obj]; goto print;
         case 'p': cs = set[_pn_psd]; goto print;
         case 's': cs = set[_pn_psi]; goto print;
         case 'r': cs = set[_pn_act]; goto print;
         print:   ln = faststrlen(cs);     faststrcpy(pt, cs); pt += ln; break;
         print_s: ln = ACS_StrLen(st); faststrcpy_str(pt, st); pt += ln; break;
         }
         ++s;
         continue;
      }

      *pt++ = *s++;
   }

   *pt = '\0';

   if(out[0]) {
      i32 time = 0;
      Dbg_Log(log_dev, _p((cstr)out));
      pl.obit = out;
   }
}

/* EOF */
