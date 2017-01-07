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
   return p->buttons & bt && !(p->old.buttons & bt);
}

bool ButtonPressedUI(player_t *p, int bt)
{
   return !(p->buttons & bt) && p->old.buttons & bt;
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
   char ch = chars[(ACS_Timer() / 4) % sizeof(chars)];
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

float normf(float x, float min, float max)
{
   return (x - min) / (max - min);
}

bool bpcldi(int x, int y, int z, int w, int x2, int y2)
{
   return x2 >= x && y2 >= y && x2 < z && y2 < w;
}

bool l1xcldi(int lx1, int lx2, int x)
{
   return x >= lx1 && x < lx2;
}

void *cpyalloc(size_t num, size_t size, void *src)
{
   void *dest = calloc(num, size);
   memcpy(dest, src, num * size);
   return dest;
}

float pymagf(float x, float y)
{
   return sqrt((x * x) + (y * y));
}

accum pymagk(accum x, accum y)
{
   return ACS_FixedSqrt((x * x) + (y * y));
}

float angle2df(float x1, float y1, float x2, float y2)
{
   float x = x2 - x1;
   float y = y2 - y1;
   return atan2(y, x);
}

accum dist2dk(accum x1, accum y1, accum x2, accum y2)
{
   return ACS_VectorLength(x1 - x2, y1 - y2);
}

float dist2df(float x1, float y1, float x2, float y2)
{
   float x = x1 - x2;
   float y = y1 - y2;
   return sqrt(x*x + y*y);
}

accum dist3dk(accum x1, accum y1, accum z1, accum x2, accum y2, accum z2)
{
   return ACS_VectorLength(ACS_VectorLength(x1 - x2, y1 - y2), z1 - z2);
}

unsigned StrHash(__str s)
{
   unsigned ret = 0;
   for(char __str_ars const *ss = s; *ss; ss++)
      ret = *ss + 101 * ret;
   return ret;
}

__str Language(__str fmt, ...)
{
   va_list vl;
   
   ACS_BeginPrint();
   
   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);
   
   __str ret = StrParam("%LS", ACS_EndStrParam());
   
   if(ret[0] == '$')
   {
      __str sub = ACS_StrMid(ret, 1, 0x7FFFFFFF);
      __str new = StrParam("%LS", sub);
      if(ACS_StrCmp(sub, new) != 0)
         ret = new;
   }
   
   return ret;
}

[[__optional_args(2)]]
bool Lith_SetPlayerVelocity(player_t *p, fixed velx, fixed vely, fixed velz, bool add, bool setbob)
{
   if(add)
   {
      p->velx += velx;
      p->vely += vely;
      p->velz += velz;
   }
   else
   {
      p->velx = velx;
      p->vely = vely;
      p->velz = velz;
   }
   
   return ACS_SetActorVelocity(p->tid, velx, vely, velz, add, setbob);
}

//
// ---------------------------------------------------------------------------

