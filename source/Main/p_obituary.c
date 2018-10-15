// Copyright © 2018 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_player.h"

script ext("ACS")
void Lith_Obituary(void)
{
   static __str const pronoun[pro_max][5] = {
      {"they", "them", "their", "theirs", "they're"},
      {"he",   "him",  "his",   "his",    "he's"},
      {"she",  "her",  "her",   "hers",   "she's"},
      {"it",   "it",   "its",   "its'",   "it's"},
   };

   struct player *p = LocalPlayer;

   __str obit = HERMES_S("GetObituary");
   if(obit == "") return;

   if(obit == "(falling)")
      obit = Language("LITH_OB_Falling_%i", ACS_Random(1, 5));
   else if(obit == "(crush)")
      obit = Language("LITH_OB_Crush_%i", ACS_Random(1, 5));
   else if(obit == "(exit)")
      obit = Language("LITH_OB_Exit_%i", ACS_Random(1, 5));
   else if(obit == "(drowning)")
      obit = Language("LITH_OB_Drowning_%i", ACS_Random(1, 5));
   else if(obit == "(slime)")
      obit = Language("LITH_OB_Slime_%i", ACS_Random(1, 5));
   else if(obit == "(fire)")
      obit = Language("LITH_OB_Fire_%i", ACS_Random(1, 5));
   else if(obit == "(suicide)")
      obit = Language("LITH_OB_Suicide_%i", ACS_Random(1, 5));
   else if(obit == "(default)")
      obit = Language("LITH_OB_Default_%i", ACS_Random(1, 5));

   ACS_BeginPrint();

   for(char __str_ars const *s = obit; *s;)
   {
      if(s[0] == '%') switch(s[1])
      {
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
   Lith_ForPlayer() p->log(1, "%S", obit);
}

// EOF
