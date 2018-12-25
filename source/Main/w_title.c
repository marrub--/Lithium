// Copyright © 2017 Alison Sanderson, all rights reserved.
#if LITHIUM
#include "lith_common.h"

StrEntON

// Static Functions ----------------------------------------------------------|

static void Blinker(void)
{
   static int const time = 20;
   static fixed a;
   static int t;

   if(t < time)
   {
      PrintTextStr("\Cd[Press any key to begin]");
      PrintTextA(s_cbifont, 0, 160,0, 220,0, a);
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
   DrawCallI("ForceDraw", true);

   ACS_Delay(35*5);

   SetSize(320, 240);
   SetClipW(0, 0, 320, 240, 310);

   char const *txt = LC(cLANG "OPENER");
   int len = strlen(txt);

   for(int t = 0, pos = 0;;)
   {
      PrintTextFmt(c"\Cd%.*s", pos, txt);
      PrintText(s_cbifont, 0, 7,1, 6,1);

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
      DrawCallI("LE");
   }

   for(int i = 0; i < 35 * 7; i++)
   {
      PrintTextFmt(c"\Cd%s", txt);
      PrintText(s_cbifont, 0, 7,1, 6,1);
      Blinker();
      ACS_Delay(1);
      DrawCallI("LE");
   }

   for(fixed a = 1; a > 0; a -= 0.01)
   {
      PrintTextFmt(c"\Cd%s", txt);
      PrintTextA(s_cbifont, 0, 7,1, 6,1, a);
      Blinker();
      ACS_Delay(1);
      DrawCallI("LE");
   }

   for(;;)
   {
      Blinker();
      ACS_Delay(1);
      DrawCallI("LE");
   }
}
#endif

// EOF
