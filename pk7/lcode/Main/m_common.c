#include "lith_common.h"
#include "lith_player.h"
#include "lith_world.h"
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#define DrawSpriteBegin(name) \
   do { \
      ACS_SetFont(name); \
      ACS_BeginPrint(); \
      ACS_PrintChar('a'); \
      ACS_MoreHudMessage(); \
   } while(0)


//----------------------------------------------------------------------------
// Static Objects
//

[[__no_init]] static crc64_t crctable[256]; // NB: Don't try to hash non-8-bit data.


//----------------------------------------------------------------------------
// Extern Objects
//

void (*DrawSprite)(__str name, int flags, int id, fixed x, fixed y, fixed hold);
void (*DrawSpriteX)(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1);
void (*DrawSpriteXX)(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2);
void (*DrawSpriteXXX)(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2, fixed a3);
void (*DrawSpritePlain)(__str name, int id, fixed x, fixed y, fixed hold);
void (*DrawSpriteAlpha)(__str name, int id, fixed x, fixed y, fixed hold, fixed alpha);
void (*DrawSpriteFade)(__str name, int id, fixed x, fixed y, fixed hold, fixed fadetime);


//----------------------------------------------------------------------------
// Static Functions
//

//
// InitCRC64
//
static void InitCRC64()
{
   crc64_t const polynomial = 0xC96C5795D7870F42; // ECMA 182

   for(crc64_t i = 0; i < 256; i++)
   {
      crc64_t remainder = i;

      for(int j = 0; j < 8; j++)
         if(remainder & 1)
            remainder = (remainder >> 1) ^ polynomial;
         else
            remainder >>= 1;

      crctable[i] = remainder;
   }
}

//
// A_DrawSprite
//
static void A_DrawSprite(__str name, int flags, int id, fixed x, fixed y, fixed hold)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage();
}

//
// A_DrawSpriteX
//
static void A_DrawSpriteX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(a1);
}

//
// A_DrawSpriteXX
//
static void A_DrawSpriteXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(a1, a2);
}

//
// A_DrawSpriteXXX
//
void A_DrawSpriteXXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2, fixed a3)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(a1, a2, a3);
}

//
// A_DrawSpritePlain
//
static void A_DrawSpritePlain(__str name, int id, fixed x, fixed y, fixed hold)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage();
}

//
// A_DrawSpriteAlpha
//
static void A_DrawSpriteAlpha(__str name, int id, fixed x, fixed y, fixed hold, fixed alpha)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_ALPHA|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(alpha);
}

//
// A_DrawSpriteFade
//
static void A_DrawSpriteFade(__str name, int id, fixed x, fixed y, fixed hold, fixed fadetime)
{
   DrawSpriteBegin(name);
   ACS_OptHudMessage(HUDMSG_FADEOUT|HUDMSG_NOTWITHFULLMAP, id, CR_UNTRANSLATED, x, y, hold);
   ACS_EndHudMessage(fadetime);
}

#if 0
//
// Z_DrawSprite
//
static void Z_DrawSprite(__str name, int flags, int id, fixed x, fixed y, fixed hold)
{
}

//
// Z_DrawSpriteX
//
static void Z_DrawSpriteX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1)
{
}

//
// Z_DrawSpriteXX
//
static void Z_DrawSpriteXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2)
{
}

//
// Z_DrawSpriteXXX
//
void Z_DrawSpriteXXX(__str name, int flags, int id, fixed x, fixed y, fixed hold, fixed a1, fixed a2, fixed a3)
{
}

//
// Z_DrawSpritePlain
//
static void Z_DrawSpritePlain(__str name, int id, fixed x, fixed y, fixed hold)
{
}

//
// Z_DrawSpriteAlpha
//
static void Z_DrawSpriteAlpha(__str name, int id, fixed x, fixed y, fixed hold, fixed alpha)
{
}

//
// Z_DrawSpriteFade
//
static void Z_DrawSpriteFade(__str name, int id, fixed x, fixed y, fixed hold, fixed fadetime)
{
}
#endif


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_CRC64
//
crc64_t Lith_CRC64(void const *data, size_t len, crc64_t result)
{
   static bool crcinit;
   unsigned char const *ptr = data;

   if(!crcinit)
      crcinit = true, InitCRC64();

   result = ~result;

   for(size_t i = 0; i < len; i++)
      result = crctable[(result ^ ptr[i]) & 0xFF] ^ (result >> 8);

   return ~result;
}

//
// RandomFloat
//
float RandomFloat(float max, float min)
{
   if(max < min)
      swap(float, min, max);

   return (ACS_Random(0, 1048576) / 1048576.f) * (max - min) + min;
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
// SetupSpriteBlitter
//
void SetupSpriteBlitter(void)
{
#if 0
   if(world.grafZoneEntered)
   {
      DrawSprite      = Z_DrawSprite;
      DrawSpriteX     = Z_DrawSpriteX;
      DrawSpriteXX    = Z_DrawSpriteXX;
      DrawSpriteXXX   = Z_DrawSpriteXXX;
      DrawSpriteAlpha = Z_DrawSpriteAlpha;
      DrawSpritePlain = Z_DrawSpritePlain;
      DrawSpriteFade  = Z_DrawSpriteFade;
   }
   else
#endif
   {
      DrawSprite      = A_DrawSprite;
      DrawSpriteX     = A_DrawSpriteX;
      DrawSpriteXX    = A_DrawSpriteXX;
      DrawSpriteXXX   = A_DrawSpriteXXX;
      DrawSpriteAlpha = A_DrawSpriteAlpha;
      DrawSpritePlain = A_DrawSpritePlain;
      DrawSpriteFade  = A_DrawSpriteFade;
   }
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
   va_list vl;

   ACS_BeginPrint();

   ACS_PrintChar('\C');
   ACS_PrintChar(chars[(ACS_Timer() / 4) % countof(chars)]);

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
// mag2f
//
float mag2f(float x, float y)
{
   return sqrtf(x * x + y * y);
}

//
// mag2k
//
fixed mag2k(fixed x, fixed y)
{
   return ACS_FixedSqrt(x * x + y * y);
}

//
// lerpk
//
fixed lerpk(fixed a, fixed b, fixed t)
{
   fixed ret = ((1.0 - t) * a) + (t * b);

   if(roundk(ret, 10) == b)
      return b;

   return ret;
}

//
// lerplk
//
fixed64_t lerplk(fixed64_t a, fixed64_t b, fixed64_t t)
{
   fixed64_t ret = ((1.0lk - t) * a) + (t * b);

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
// bpcldi
//
bool bpcldi(int x, int y, int z, int w, int x2, int y2)
{
   return x2 >= x && y2 >= y && x2 < z && y2 < w;
}

//
// ceilk
//
int ceilk(fixed n)
{
   union {int_k_t i; fixed a;} u = {.a = n};
   if(u.i & 0xFFF1) return u.i &= 0xFFFF0000, u.a + 1;
   else             return (int)u.a;
}

#define StrHashImpl() \
   unsigned ret = 0; \
   for(; *s; s++) ret = *s + 101 * ret; \
   return ret

//
// StrHash
//
unsigned StrHash(char __str_ars const *s)
{
   StrHashImpl();
}

//
// CStrHash
//
unsigned CStrHash(char const *s)
{
   StrHashImpl();
}

//
// LanguageV
//
static __str LanguageV(__str name)
{
   __str ret = StrParam("%LS", name);

   if(ret[0] == '$')
   {
      __str sub = ACS_StrMid(ret, 1, 0x7FFFFFFF);
      __str nex = StrParam("%LS", sub);
      if(sub != nex)
         ret = nex;
   }

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

   return LanguageV(ACS_EndStrParam());
}

//
// LanguageNull
//
__str LanguageNull(__str fmt, ...)
{
   va_list vl;

   ACS_BeginPrint();

   va_start(vl, fmt);
   __vnprintf_str(fmt, vl);
   va_end(vl);

   __str name = ACS_EndStrParam();
   __str alias = LanguageV(name);

   return name == alias ? null : alias;

}

//
// Lith_GetTID
//
[[__call("ScriptS")]]
int Lith_GetTID(int tid, int ptr)
{
   if(tid || ptr)
      ACS_SetActivator(tid, ptr);
   return ACS_ActivatorTID();
}

//
// Lith_GetPlayerNumber
//
[[__call("ScriptS")]]
int Lith_GetPlayerNumber(int tid, int ptr)
{
   if(tid || ptr)
      ACS_SetActivator(tid, ptr);
   return ACS_PlayerNumber();
}

//
// Lith_ValidPointer
//
[[__call("ScriptS")]]
bool Lith_ValidPointer(int tid, int ptr)
{
   if(tid || ptr)
      return ACS_SetActivator(tid, ptr);
   else
      return true;
}

//
// Lith_SetPointer
//
[[__call("ScriptS")]]
bool Lith_SetPointer(int tid, int ptr, int assign, int tid2, int ptr2, int flags)
{
   if(tid || ptr)
      ACS_SetActivator(tid, ptr);
   return ACS_SetPointer(assign, tid2, ptr2, flags);
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
   char out[48] = {};

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

#define StrCmpImpl() \
   for(; *s1 && *s2; ++s1, ++s2) {if(*s1 != *s2) return *s1 - *s2;} \
   return *s1 - *s2

//
// Lith_strcmp_str
//
int Lith_strcmp_str(char const *s1, char __str_ars const *s2)
{
   StrCmpImpl();
}

// EOF

