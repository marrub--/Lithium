// Copyright © 2018 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "common.h"
#include "p_player.h"

script ext("ACS")
void Lith_Obituary(void)
{
   static char const *pronoun[pro_max][5] = {
      {"they", "them", "their", "theirs", "they're"},
      {"she",  "her",  "her",   "hers",   "she's"  },
      {"he",   "him",  "his",   "his",    "he's"   },
      {"it",   "it",   "its",   "its'",   "it's"   },
   };

   struct player *p = LocalPlayer;

   str obit = ServCallS(s"GetObituary");
   if(obit == s_NIL) return;

   i32 rn = ACS_Random(1, 5);

        if(obit == s_ob_crush)    obit = Language(LANG "OB_Crush_%i",    rn);
   else if(obit == s_ob_default)  obit = Language(LANG "OB_Default_%i",  rn);
   else if(obit == s_ob_drowning) obit = Language(LANG "OB_Drowning_%i", rn);
   else if(obit == s_ob_exit)     obit = Language(LANG "OB_Exit_%i",     rn);
   else if(obit == s_ob_falling)  obit = Language(LANG "OB_Falling_%i",  rn);
   else if(obit == s_ob_fire)     obit = Language(LANG "OB_Fire_%i",     rn);
   else if(obit == s_ob_slime)    obit = Language(LANG "OB_Slime_%i",    rn);
   else if(obit == s_ob_suicide)  obit = Language(LANG "OB_Suicide_%i",  rn);

   noinit static char out[1024];
   char *pt = out;

   for(char __str_ars const *s = obit; *s;)
   {
      char const *cs;
      str st;
      i32 len;

      if(s[0] == '%') switch(s[1]) {
      case 'o': s += 2; st = p->name;                goto print_s;
      case 'g': s += 2; cs = pronoun[p->pronoun][0]; goto print;
      case 'h': s += 2; cs = pronoun[p->pronoun][1]; goto print;
      case 'p': s += 2; cs = pronoun[p->pronoun][2]; goto print;
      case 's': s += 2; cs = pronoun[p->pronoun][3]; goto print;
      case 'r': s += 2; cs = pronoun[p->pronoun][4]; goto print;
      print:   len =     strlen(cs);      strcpy(pt, cs); pt += len; continue;
      print_s: len = ACS_StrLen(st); lstrcpy_str(pt, st); pt += len; continue;
      }

      *pt++ = *s++;
   }

   *pt = '\0';

   if(obit != s_NIL) {
      LogDebug(log_dev, "%s", out);
      Lith_ForPlayer() p->logB(1, "%s", out);
   }
}
#endif

// EOF
