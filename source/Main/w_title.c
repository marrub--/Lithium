// Copyright © 2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"

// Static Functions ----------------------------------------------------------|

static void Blinker(void)
{
   static int const time = 20;
   static fixed a;
   static int t;

   if(t < time)
   {
      PrintTextStr("\Cd[Press any key to begin]");
      PrintTextA("cbifont", 0, 160,0, 220,0, a);
      a += 0.006;
   }
   else if(t >= time*2)
   {
      t = 0;
      a += 0.004;
   }

   if(a > 1) a = 1;

   t++;
}

// Extern Functions ----------------------------------------------------------|

script
void Lith_Title(void)
{
   URANUS("ForceDraw", true);

   ACS_Delay(35*5);

   SetSize(320, 240);
   SetClipW(0, 0, 320, 240, 310);

   __str txt = L("LITH_OPENER");
   int   len = ACS_StrLen(txt);
   int   pos = 0;

   for(int t = 0;;)
   {
      PrintTextFmt("\Cd%.*S", pos, txt);
      PrintText("cbifont", 0, 7,1, 6,1);

      if(t == 0)
      {
         if(++pos >= len) break;

         if(txt[pos] == '\n')
            t = 20;
         else
            t = 1;
      }
      else
         t--;

      Blinker();
      ACS_Delay(1);
      URANUS("LE");
   }

   for(int i = 0; i < 35 * 7; i++)
   {
      PrintTextFmt("\Cd%S", txt);
      PrintText("cbifont", 0, 7,1, 6,1);
      Blinker();
      ACS_Delay(1);
      URANUS("LE");
   }

   for(fixed a = 1; a > 0; a -= 0.01)
   {
      PrintTextFmt("\Cd%S", txt);
      PrintTextA("cbifont", 0, 7,1, 6,1, a);
      Blinker();
      ACS_Delay(1);
      URANUS("LE");
   }

   for(;;)
   {
      Blinker();
      ACS_Delay(1);
      URANUS("LE");
   }
}

// EOF
