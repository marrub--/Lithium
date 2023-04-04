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

#include "m_engine.h"

static
void Blinker(str bgn) {
   enum {time = 20};
   noinit static k32 a;
   noinit static i32 t;
   if(t < time) {
      PrintText_str(bgn, sf_smallfnt, Cr(green), 160,0, 220,0, _u_alpha, a, 310);
      a += 0.006;
   } else if(t >= time*2) {
      t = 0;
      a += 0.004;
   }
   if(a > 1) a = 1;
   t++;
}

dynam_aut script void W_Title(void) {
   FadeFlash(0, 0, 0, 1.0k, 5.0k);
   ACS_SetMusic(sp_lmusic_Shredder);
   ACS_Delay(35*5);
   SetSize(320, 240);
   str txt = ns(lang(sl_opener));
   str bgn = ns(lang(sl_begin));
   i32 len = ACS_StrLen(txt);
   for(i32 t = 0, pos = 0;;) {
      PrintText_str(ACS_StrMid(txt, 0, pos), sf_smallfnt, Cr(green), 7,1, 6,1, 0, 0, 310);
      if(t == 0) {
         if(++pos >= len) break;
         if(txt[pos] == '\n') {
            t = 20;
         } else {
            t = 1;
         }
      } else {
         t--;
      }
      Blinker(bgn);
      ACS_Delay(1);
   }
   for(i32 i = 0; i < 35 * 7; i++) {
      PrintText_str(txt, sf_smallfnt, Cr(green), 7,1, 6,1, 0, 0, 310);
      Blinker(bgn);
      ACS_Delay(1);
   }
   for(k32 a = 1; a > 0; a -= 0.01) {
      PrintText_str(txt, sf_smallfnt, Cr(green), 7,1, 6,1, _u_alpha, a, 310);
      Blinker(bgn);
      ACS_Delay(1);
   }
   for(;;) {
      Blinker(bgn);
      ACS_Delay(1);
   }
}

/* EOF */
