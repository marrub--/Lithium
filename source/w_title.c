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

enum {
   _scr_size_x = 640,
   _scr_size_y = 480,
   _scr_left = _scr_size_x / 45,
   _scr_right = _scr_size_x - _scr_left,
   _scr_top = _scr_size_y / 54,
};

static void Blinker(str bgn) {
   enum {time = 20};
   noinit static k32 a;
   noinit static i32 t;
   if(t < time) {
      PrintText_str(bgn, sf_bigupper, Cr(green), _scr_size_x/2,0, _scr_size_y-_scr_size_y/12,0, _u_alpha, a, _scr_right);
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
   str txt = ns(lang(sl_opener));
   str bgn = ns(lang(sl_begin));
   i32 len = ACS_StrLen(txt);
   for(i32 t = 0, pos = 0;;) {
      SetSize(_scr_size_x, _scr_size_y);
      PrintText_str(ACS_StrMid(txt, 0, pos), sf_bigupper, Cr(green), _scr_left,1, _scr_top,1, 0, 0, _scr_right);
      if(t == 0) {
         if(++pos >= len) break;
         if(txt[pos] != '\n' || (pos > 0 && txt[pos - 1] != '\n')) {
            play_key_snd(txt[pos]);
         }
         switch(txt[pos]) {
         case '\n': t = 20; break;
         case '!':
         case '?':
         case '.': t = 5; break;
         case ';':
         case ':': t = 3; break;
         case ',': t = 2; break;
         default:  t = 1; break;
         }
      } else {
         t--;
      }
      Blinker(bgn);
      ACS_Delay(1);
   }
   for(i32 i = 0; i < 35 * 7; i++) {
      SetSize(_scr_size_x, _scr_size_y);
      PrintText_str(txt, sf_bigupper, Cr(green), _scr_left,1, _scr_top,1, 0, 0, _scr_right);
      Blinker(bgn);
      ACS_Delay(1);
   }
   for(k32 a = 1; a > 0; a -= 0.01) {
      SetSize(_scr_size_x, _scr_size_y);
      PrintText_str(txt, sf_bigupper, Cr(green), _scr_left,1, _scr_top,1, _u_alpha, a, _scr_right);
      Blinker(bgn);
      ACS_Delay(1);
   }
   for(;;) {
      SetSize(_scr_size_x, _scr_size_y);
      Blinker(bgn);
      ACS_Delay(1);
   }
}

/* EOF */
