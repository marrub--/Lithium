#include "lith_common.h"
#include "lith_player.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>

// ---------------------------------------------------------------------------
// Functions.
//

bool ButtonPressed(player_t *p, int bt)
{
   return p->buttons & bt && !(p->oldbuttons & bt);
}

int Random(int max, int min)
{
   if(max < min)
   {
      int temp = min;
      min = max;
      max = temp;
   }
   
   return min + (rand() % (max - min + 1));
}

fixed RandomFixed(fixed max, fixed min)
{
   return kbits(Random(bitsk(max), bitsk(min)));
}

float RandomFloat(float max, float min)
{
   if(max < min)
   {
      float temp = min;
      min = max;
      max = temp;
   }
   
   return (rand() / (float)RAND_MAX) * (max - min) + min;
}

__str StrParam(__str fmt, ...)
{
   va_list vl;
   
   ACS_BeginPrint();
   
   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);
   
   return ACS_EndStrParam();
}

void HudMessage(__str fmt, ...)
{
   va_list vl;
   
   ACS_BeginPrint();
   
   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);
   
   ACS_MoreHudMessage();
}

void HudMessageRainbows(__str fmt, ...)
{
   static char const chars[] = { 'g', 'i', 'k', 'd', 'h', 't', 'r' };
   static int const charsmax = sizeof(chars);
   
   char ch = chars[(ACS_Timer() / 4) % charsmax]; // GDCC bug: sizeof doesn't work here
   va_list vl;
   
   ACS_BeginPrint();
   
   ACS_PrintChar('\C');
   ACS_PrintChar(ch);
   
   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);
   
   ACS_MoreHudMessage();
}

void Log(__str fmt, ...)
{
   va_list vl;
   
   ACS_BeginPrint();
   
   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);
   
   ACS_EndLog();
}

__str StrUpper(__str in)
{
   ACS_BeginStrParam();
   
   for(char __str_ars const *c = in; *c; c++)
      ACS_PrintChar(toupper(*c));
   
   return ACS_EndStrParam();
}

accum lerpk(accum a, accum b, accum t)
{
   accum ret = ((1.0 - t) * a) + (t * b);
   
   if(roundk(ret, 10) == b)
   {
      return b;
   }
   
   return ret;
}

float lerpf(float a, float b, float t)
{
   float ret = ((1.0 - t) * a) + (t * b);
   
   if((round(ret << 10) >> 10) == b)
   {
      return b;
   }
   
   return ret;
}

//
// ---------------------------------------------------------------------------

