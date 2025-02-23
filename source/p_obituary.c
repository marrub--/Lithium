// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Obituary string formatting and dispatch.                                 │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
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
void Z_Obituary(void) {
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
      for(char *next = nil, *word = faststrtok(pbuf, &next, '/');
          word && i < _pn_max; word = faststrtok(nil, &next, '/')) {
         set[i++] = word;
      }
   }

   str ob = EDataS(_edt_obituary);
   if(!ob[0]) return;

   i32 rn = ACS_Random(1, 5);

   #define OB LANG "OB_"
   /**/ if(ob == st_ob_crush)    ob = lang(strp(_l(OB "Crush_"),    _p(rn)));
   else if(ob == st_ob_default)  ob = lang(strp(_l(OB "Default_"),  _p(rn)));
   else if(ob == st_ob_drowning) ob = lang(strp(_l(OB "Drowning_"), _p(rn)));
   else if(ob == st_ob_exit)     ob = lang(strp(_l(OB "Exit_"),     _p(rn)));
   else if(ob == st_ob_falling)  ob = lang(strp(_l(OB "Falling_"),  _p(rn)));
   else if(ob == st_ob_fire)     ob = lang(strp(_l(OB "Fire_"),     _p(rn)));
   else if(ob == st_ob_slime)    ob = lang(strp(_l(OB "Slime_"),    _p(rn)));
   else if(ob == st_ob_suicide)  ob = lang(strp(_l(OB "Suicide_"),  _p(rn)));
   ob = ns(ob);
   #undef OB

   noinit static
   char out[1024];

   char *pt = out;

   for(astr s = ns(ob); *s;) {
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
