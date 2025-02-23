// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
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

static void Blinker() {
   enum {time = 20};
   noinit static k32 a;
   noinit static i32 t;
   if(t < time) {
      PrintText_str(sl_begin, sf_bigupper, Cr(green), _scr_size_x/2,0, _scr_size_y-_scr_size_y/12,0, _u_alpha, a, _scr_right);
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
   if(cv.sv_notitleintro) {
      return;
   }
   ACS_Delay(35*5);
   i32 len = ACS_StrLen(sl_opener);
   for(i32 t = 0, pos = 0;;) {
      SetSize(_scr_size_x, _scr_size_y);
      PrintText_str(ACS_StrMid(sl_opener, 0, pos), sf_bigupper, Cr(green), _scr_left,1, _scr_top,1, 0, 0, _scr_right);
      if(t == 0) {
         if(++pos >= len) break;
         if(sl_opener[pos] != '\n' || (pos > 0 && sl_opener[pos - 1] != '\n')) {
            play_key_snd(sl_opener[pos]);
         }
         switch(sl_opener[pos]) {
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
      Blinker();
      ACS_Delay(1);
   }
   for(i32 i = 0; i < 35 * 7; i++) {
      SetSize(_scr_size_x, _scr_size_y);
      PrintText_str(sl_opener, sf_bigupper, Cr(green), _scr_left,1, _scr_top,1, 0, 0, _scr_right);
      Blinker();
      ACS_Delay(1);
   }
   for(k32 a = 1; a > 0; a -= 0.01) {
      SetSize(_scr_size_x, _scr_size_y);
      PrintText_str(sl_opener, sf_bigupper, Cr(green), _scr_left,1, _scr_top,1, _u_alpha, a, _scr_right);
      Blinker();
      ACS_Delay(1);
   }
   for(;;) {
      SetSize(_scr_size_x, _scr_size_y);
      Blinker();
      ACS_Delay(1);
   }
}

/* EOF */
