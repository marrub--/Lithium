// Copyright © 2018 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "lith_common.h"
#include "lith_player.h"

StrEntOFF

script ext("ACS")
void Lith_Obituary(void)
{
   static char const *pronoun[pro_max][5] = {
      {"they", "them", "their", "theirs", "they're"},
      {"she",  "her",  "her",   "hers",   "she's"  },
      {"he",   "him",  "his",   "his",    "he's"   },
      {"it",   "it",   "its",   "its'",   "it's"   },
   };

   StrEntON
   struct player *p = LocalPlayer;

   __str obit = ServCallS("GetObituary");
   if(obit == "") return;

   int rn = ACS_Random(1, 5);

        if(obit == "(falling)")  obit = Language(LANG "OB_Falling_%i",  rn);
   else if(obit == "(crush)")    obit = Language(LANG "OB_Crush_%i",    rn);
   else if(obit == "(exit)")     obit = Language(LANG "OB_Exit_%i",     rn);
   else if(obit == "(drowning)") obit = Language(LANG "OB_Drowning_%i", rn);
   else if(obit == "(slime)")    obit = Language(LANG "OB_Slime_%i",    rn);
   else if(obit == "(fire)")     obit = Language(LANG "OB_Fire_%i",     rn);
   else if(obit == "(suicide)")  obit = Language(LANG "OB_Suicide_%i",  rn);
   else if(obit == "(default)")  obit = Language(LANG "OB_Default_%i",  rn);

   ACS_BeginPrint();

   for(char __str_ars const *s = obit; *s;)
   {
      char const *pr;
      if(s[0] == '%') switch(s[1]) {
      case 'o': s += 2; ACS_PrintName(p->num+1); continue;
      case 'g': s += 2; pr = pronoun[p->pronoun][0]; goto print;
      case 'h': s += 2; pr = pronoun[p->pronoun][1]; goto print;
      case 'p': s += 2; pr = pronoun[p->pronoun][2]; goto print;
      case 's': s += 2; pr = pronoun[p->pronoun][3]; goto print;
      case 'r': s += 2; pr = pronoun[p->pronoun][4]; goto print;
      print: PrintChars(pr, strlen(pr)); continue;
      }

      ACS_PrintChar(*(s++));
   }

   obit = ACS_EndStrParam();

   if(obit != "") {
      LogDebug(log_dev, "%S", obit);
      Lith_ForPlayer() p->logB(1, "%S", obit);
   }
}
#endif

// EOF
