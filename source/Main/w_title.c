// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// w_title.c: Title map scripts.

#if LITHIUM
#include "common.h"

// Static Functions ----------------------------------------------------------|

static void Blinker(void)
{
   static i32 const time = 20;
   static k32 a;
   static i32 t;

   if(t < time)
   {
      PrintTextA_str(st_begin, s_cbifont, 0, 160,0, 220,0, a);
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
void W_Title(void)
{
   DrawCallI(sm_ForceDraw, true);

   ACS_Delay(35*5);

   SetSize(320, 240);
   SetClipW(0, 0, 320, 240, 310);

   char const *txt = LC(LANG "OPENER");
   i32 len = strlen(txt);

   for(i32 t = 0, pos = 0;;)
   {
      PrintTextFmt("\Cd%.*s", pos, txt);
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
      DrawCallI(sm_LE);
   }

   for(i32 i = 0; i < 35 * 7; i++)
   {
      PrintTextFmt("\Cd%s", txt);
      PrintText(s_cbifont, 0, 7,1, 6,1);
      Blinker();
      ACS_Delay(1);
      DrawCallI(sm_LE);
   }

   for(k32 a = 1; a > 0; a -= 0.01)
   {
      PrintTextFmt("\Cd%s", txt);
      PrintTextA(s_cbifont, 0, 7,1, 6,1, a);
      Blinker();
      ACS_Delay(1);
      DrawCallI(sm_LE);
   }

   for(;;)
   {
      Blinker();
      ACS_Delay(1);
      DrawCallI(sm_LE);
   }
}
#endif

// EOF
