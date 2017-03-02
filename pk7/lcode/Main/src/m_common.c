#include "lith_common.h"
#include "lith_player.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>

//----------------------------------------------------------------------------
// Extern Functions
//

//
// ButtonPressed
//
bool ButtonPressed(player_t *p, int bt)
{
   return p->buttons & bt && !(p->old.buttons & bt);
}

//
// ButtonPressedUI
//
// In world stuff, generally it's expected that something is pressed when
// you hit the button, immediately.  However, with UIs, generally we expect
// things to be clicked when the button is released.
//
bool ButtonPressedUI(player_t *p, int bt)
{
   return !(p->buttons & bt) && p->old.buttons & bt;
}

//
// RandomFloat
//
float RandomFloat(float max, float min)
{
   if(max < min)
   {
      float temp = min;
      min = max;
      max = temp;
   }
   
   return ACS_RandomFixed(0, 1) * (max - min) + min;
}

//
// StrParam
//
__str StrParam(__str fmt, ...)
{
   va_list vl;
   
   ACS_BeginPrint();
   
   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);
   
   return ACS_EndStrParam();
}

//
// HudMessage
//
void HudMessage(__str fmt, ...)
{
   va_list vl;
   
   ACS_BeginPrint();
   
   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);
   
   ACS_MoreHudMessage();
}

//
// HudMessageRainbows
//
void HudMessageRainbows(__str fmt, ...)
{
   static char const chars[] = {'g', 'i', 'k', 'd', 'h', 't', 'r'};
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

//
// Log
//
void Log(__str fmt, ...)
{
   va_list vl;
   
   ACS_BeginPrint();
   
   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);
   
   ACS_EndLog();
}

//
// PrintBold
//
void PrintBold(__str fmt, ...)
{
   va_list vl;
   
   ACS_BeginPrint();
   
   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);
   
   ACS_EndPrintBold();
}

//
// StrUpper
//
__str StrUpper(__str in)
{
   ACS_BeginStrParam();
   
   for(char __str_ars const *c = in; *c; c++)
      ACS_PrintChar(toupper(*c));
   
   return ACS_EndStrParam();
}

//
// lerpk
//
accum lerpk(accum a, accum b, accum t)
{
   accum ret = ((1.0 - t) * a) + (t * b);
   
   if(roundk(ret, 10) == b)
      return b;
   
   return ret;
}

//
// lerplk
//
long accum lerplk(long accum a, long accum b, long accum t)
{
   long accum ret = ((1.0lk - t) * a) + (t * b);
   
   if(roundlk(ret, 10) == b)
      return b;
   
   return ret;
}

//
// lerpf
//
float lerpf(float a, float b, float t)
{
   float ret = ((1.0 - t) * a) + (t * b);
   
   if((round(ret << 10) >> 10) == b)
      return b;
   
   return ret;
}

//
// normf
//
float normf(float x, float min, float max)
{
   return (x - min) / (max - min);
}

//
// bpcldi
//
bool bpcldi(int x, int y, int z, int w, int x2, int y2)
{
   return x2 >= x && y2 >= y && x2 < z && y2 < w;
}

//
// l1xcldi
//
bool l1xcldi(int lx1, int lx2, int x)
{
   return x >= lx1 && x < lx2;
}

//
// cpyalloc
//
void *cpyalloc(size_t num, size_t size, void *src)
{
   void *dest = calloc(num, size);
   memcpy(dest, src, num * size);
   return dest;
}

//
// pymagf
//
float pymagf(float x, float y)
{
   return sqrt((x * x) + (y * y));
}

//
// pymagk
//
accum pymagk(accum x, accum y)
{
   return ACS_FixedSqrt((x * x) + (y * y));
}

//
// angle2df
//
float angle2df(float x1, float y1, float x2, float y2)
{
   float x = x2 - x1;
   float y = y2 - y1;
   return atan2(y, x);
}

//
// dist2dk
//
accum dist2dk(accum x1, accum y1, accum x2, accum y2)
{
   return ACS_VectorLength(x1 - x2, y1 - y2);
}

//
// dist2df
//
float dist2df(float x1, float y1, float x2, float y2)
{
   float x = x1 - x2;
   float y = y1 - y2;
   return pymagf(x, y);
}

//
// dist3dk
//
accum dist3dk(accum x1, accum y1, accum z1, accum x2, accum y2, accum z2)
{
   return ACS_VectorLength(dist2dk(x1, x2, y1, y2), z1 - z2);
}

//
// ceilk
//
int ceilk(accum n)
{
   union {int_k_t i; accum a;} u = {.a = n};
   if(u.i & 0xFFF1) return u.i &= 0xFFFF0000, u.a + 1;
   else             return (int)u.a;
}

//
// StrHash
//
unsigned StrHash(__str s)
{
   unsigned ret = 0;
   for(char __str_ars const *ss = s; *ss; ss++)
      ret = *ss + 101 * ret;
   return ret;
}

//
// Language
//
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

//
// Lith_SetPlayerVelocity
//
bool Lith_SetPlayerVelocity(player_t *p, fixed velx, fixed vely, fixed velz, bool add, bool setbob)
{
   if(add)
      p->velx += velx, p->vely += vely, p->velz += velz;
   else
      p->velx = velx, p->vely = vely, p->velz = velz;
   
   return ACS_SetActorVelocity(p->tid, velx, vely, velz, add, setbob);
}

//
// Lith_GetTID
//
[[__call("ScriptS")]]
int Lith_GetTID(int tid, int ptr)
{
   ACS_SetActivator(tid, ptr);
   return ACS_ActivatorTID();
}

//
// Lith_GetPlayerNumber
//
[[__call("ScriptS")]]
int Lith_GetPlayerNumber(int tid, int ptr)
{
   ACS_SetActivator(tid, ptr);
   return ACS_PlayerNumber();
}

//
// Lith_CheckActorInventory
//
int Lith_CheckActorInventory(int tid, __str item)
{
   if(tid == 0) return ACS_CheckInventory(item);
   else         return ACS_CheckActorInventory(tid, item);
}

//
// Lith_GiveActorInventory
//
void Lith_GiveActorInventory(int tid, __str item, int amount)
{
   if(tid == 0) ACS_GiveInventory(item, amount);
   else         ACS_GiveActorInventory(tid, item, amount);
}

//
// Lith_ScoreSep
//
__str Lith_ScoreSep(score_t num)
{
   static char out[48];
   
   if(num)
   {
      char *outp = out + 47;
      int cnum = 0;
      
      while(num)
      {
         *--outp = (num % 10) + '0';
         num = num / 10;
         
         if(++cnum == 3)
         {
            *--outp = ',';
            cnum = 0;
         }
      }
      
      if(!cnum) outp++;
      
      return StrParam("%s", outp);
   }
   else
      return "0";
}

//
// Lith_strcpy_str
//
char *Lith_strcpy_str(char *dest, char __str_ars const *src)
{
   for(char *i = dest; (*i = *src); ++i, ++src);
   return dest;
}

//
// Lith_strcmp_str
//
int Lith_strcmp_str(char const *s1, char __str_ars const *s2)
{
   for(; *s1 && *s2; ++s1, ++s2)
   {
      if(*s1 != *s2)
         return *s1 - *s2;
   }

   return *s1 - *s2;
}

// EOF

