// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Title map scripts.                                                       │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"

/* Static Functions -------------------------------------------------------- */

static
void Blinker(str bgn) {
   static
   i32 const time = 20;

   noinit static
   k32 a;

   noinit static
   i32 t;

   if(t < time) {
      PrintTextA_str(bgn, sf_smallfnt, Cr(green), 160,0, 220,0, a);
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
   ACS_FadeTo(0, 0, 0, 1.0k, 0);
   ACS_FadeTo(0, 0, 0, 0, 5.0k);
   ACS_SetMusic(ACS_Random(0, 1) ? sp_lmusic_Shredder : sp_lmusic_miniwip2);

   ACS_Delay(35*5);

   SetSize(320, 240);
   SetClipW(0, 0, 320, 240, 310);

   str txt = ns(lang(sl_opener));
   str bgn = ns(lang(sl_begin));

   i32 len = ACS_StrLen(txt);

   for(i32 t = 0, pos = 0;;)
   {
      PrintText_str(ACS_StrMid(txt, 0, pos), sf_smallfnt, Cr(green), 7,1, 6,1);

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
      EndDrawing();
   }

   for(i32 i = 0; i < 35 * 7; i++)
   {
      PrintText_str(txt, sf_smallfnt, Cr(green), 7,1, 6,1);
      Blinker(bgn);
      ACS_Delay(1);
      EndDrawing();
   }

   for(k32 a = 1; a > 0; a -= 0.01)
   {
      PrintTextA_str(txt, sf_smallfnt, Cr(green), 7,1, 6,1, a);
      Blinker(bgn);
      ACS_Delay(1);
      EndDrawing();
   }

   for(;;)
   {
      Blinker(bgn);
      ACS_Delay(1);
      EndDrawing();
   }
}

/* EOF */
