// Copyright © 2018 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"

StrEntON

script ext("ACS")
void Lith_Obituary(void)
{
   static __str const pronoun[pro_max][5] = {
      {"they", "them", "their", "theirs", "they're"},
      {"she",  "her",  "her",   "hers",   "she's"  },
      {"he",   "him",  "his",   "his",    "he's"   },
      {"it",   "it",   "its",   "its'",   "it's"   },
   };

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
      if(s[0] == '%') switch(s[1]) {
      case 'o': s += 2; ACS_PrintName(p->num+1);                 continue;
      case 'g': s += 2; ACS_PrintString(pronoun[p->pronoun][0]); continue;
      case 'h': s += 2; ACS_PrintString(pronoun[p->pronoun][1]); continue;
      case 'p': s += 2; ACS_PrintString(pronoun[p->pronoun][2]); continue;
      case 's': s += 2; ACS_PrintString(pronoun[p->pronoun][3]); continue;
      case 'r': s += 2; ACS_PrintString(pronoun[p->pronoun][4]); continue;
      }

      ACS_PrintChar(*(s++));
   }

   obit = ACS_EndStrParam();

   LogDebug(log_dev, "%S", obit);
   Lith_ForPlayer() p->logB(1, "%S", obit);
}

// EOF
