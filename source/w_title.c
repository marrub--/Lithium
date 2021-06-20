/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Title map scripts.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"

/* Static Functions -------------------------------------------------------- */

static
void Blinker(cstr bgn) {
   static
   i32 const time = 20;

   noinit static
   k32 a;

   noinit static
   i32 t;

   if(t < time) {
      PrintTextChS(bgn);
      PrintTextA(sf_smallfnt, Cr(green), 160,0, 220,0, a);
      a += 0.006;
   } else if(t >= time*2) {
      t = 0;
      a += 0.004;
   }

   if(a > 1) a = 1;

   t++;
}

/* Extern Functions -------------------------------------------------------- */

dynam_aut script
void W_Title(void) {
   DrawCallI(sm_ForceDraw, true);

   ACS_Delay(35*5);

   SetSize(320, 240);
   SetClipW(0, 0, 320, 240, 310);

   char *txt = Malloc(4096, _tag_ttle);
   char *bgn = Malloc(128,  _tag_ttle);
   LanguageCV(txt, LANG "OPENER");
   LanguageCV(bgn, LANG "BEGIN");

   i32 len = faststrlen(txt);

   for(i32 t = 0, pos = 0;;)
   {
      PrintTextChr(txt, pos);
      PrintText(sf_smallfnt, Cr(green), 7,1, 6,1);

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

      Blinker(bgn);
      ACS_Delay(1);
      DrawCallI(sm_LE);
   }

   for(i32 i = 0; i < 35 * 7; i++)
   {
      PrintTextChS(txt);
      PrintText(sf_smallfnt, Cr(green), 7,1, 6,1);
      Blinker(bgn);
      ACS_Delay(1);
      DrawCallI(sm_LE);
   }

   for(k32 a = 1; a > 0; a -= 0.01)
   {
      PrintTextChS(txt);
      PrintTextA(sf_smallfnt, Cr(green), 7,1, 6,1, a);
      Blinker(bgn);
      ACS_Delay(1);
      DrawCallI(sm_LE);
   }

   for(;;)
   {
      Blinker(bgn);
      ACS_Delay(1);
      DrawCallI(sm_LE);
   }
}

/* EOF */
