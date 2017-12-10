// Copyright © 2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"

// Types ---------------------------------------------------------------------|

enum
{
   hid_t_begin = 10,
   hid_t_blinker,
   hid_t_opener,
};

// Static Functions ----------------------------------------------------------|

//
// Blinker
//
[[__call("ScriptS")]]
void Blinker(void)
{
   static int const time = 20;

   fixed a = 0;

   for(int t = 0;; t++)
   {
      if(t < time)
      {
         HudMessage("\Cd[Press any key to begin]");
         HudMessageAlpha(hid_t_blinker, 0.5, 0.8, TS, a);
         a += 0.006;
      }
      else if(t >= time*2)
      {
         t = 0;
         a += 0.004;
      }

      if(a > 1) a = 1;

      ACS_Delay(1);
   }
}

// Extern Functions ----------------------------------------------------------|

//
// Lith_Title
//
[[__call("ScriptS")]]
void Lith_Title(void)
{
   ACS_Delay(35*5);

   Blinker();

   ACS_SetHudSize(320, 240);

   __str txt = Language("LITH_TXT_OPENER");
   int   len = ACS_StrLen(txt);
   int   pos = 0;

   int t = 0;

   for(;;)
   {
      if(pos < len) pos++;

      HudMessage("%.*S", pos, txt);
      HudMessageParams(HUDMSG_FADEOUT, hid_t_opener, CR_GREEN, 7.1, 6.1, 7, 7.0);

      if(pos == len)
         return;

      ACS_Delay(txt[pos] == '\n' ? 20 : 1);
   }
}

// EOF
