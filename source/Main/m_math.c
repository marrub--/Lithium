// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"

#include <math.h>

#define BezierImpl(type, ret, func) \
   type xa = func(x1, x2, t); \
   type ya = func(y1, y2, t); \
   type xb = func(x2, x3, t); \
   type yb = func(y2, y3, t); \
   return (ret){func(xa, xb, t), func(ya, yb, t)}

static u64 lmvar crctable[256]; // NB: Don't try to hash >8bit data.
static bool lmvar crcinit;

static void InitCRC64()
{
   u64 const polynomial = 0xC96C5795D7870F42; // ECMA 182

   for(u64 i = 0; i < 256; i++)
   {
      u64 remainder = i;

      for(int j = 0; j < 8; j++) {
         if(remainder & 1) remainder = (remainder >> 1) ^ polynomial;
         else              remainder >>= 1;
      }

      crctable[i] = remainder;
   }

   crcinit = true;
}

u64 crc64(void const *data, size_t len, u64 result)
{
   byte const *ptr = data;

   if(!crcinit) InitCRC64();

   result = ~result;

   for(size_t i = 0; i < len; i++)
      result = crctable[(result ^ ptr[i]) & 0xFF] ^ (result >> 8);

   return ~result;
}

u64 crc64_str(void  __str_ars const *data, size_t len, u64 result)
{
   byte __str_ars const *ptr = data;

   if(!crcinit) InitCRC64();

   result = ~result;

   for(size_t i = 0; i < len; i++)
      result = crctable[(result ^ ptr[i]) & 0xFF] ^ (result >> 8);

   return ~result;
}

stkcall
fixed64 powlk(fixed64 x, int y)
{
   fixed64 z = 1;
   while(y-- > 0) {z *= x;}
   return z;
}

stkcall
fixed64 mag2lk(fixed64 x, fixed64 y)
{
   return ACS_FixedSqrt(x * x + y * y);
}

stkcall
fixed mag2k(fixed x, fixed y)
{
   return ACS_FixedSqrt(x * x + y * y);
}

stkcall
int mag2i(int x, int y)
{
   return ACS_Sqrt(x * x + y * y);
}

fixed lerpk(fixed a, fixed b, fixed t)
{
   fixed ret = ((1.0k - t) * a) + (t * b);

   if(roundk(ret, 15) == b)
      return b;

   return ret;
}

fixed64 lerplk(fixed64 a, fixed64 b, fixed64 t)
{
   fixed64 ret = ((1.0lk - t) * a) + (t * b);

   if(roundlk(ret, 15) == b)
      return b;

   return ret;
}

stkcall
bool aabb(int x, int y, int z, int w, int x2, int y2)
{
   return x2 >= x && y2 >= y && x2 < z && y2 < w;
}

stkcall
int ceilk(fixed n)
{
   union {int_k_t i; fixed a;} u = {.a = n};
   if(u.i & 0xFFF1) return u.i &= 0xFFFF0000, u.a + 1;
   else             return (int)u.a;
}

stkcall
fixed64 bzpolylk(fixed64 a, fixed64 b, fixed64 t)
{
   return a + ((b - a) * t);
}

stkcall
int bzpolyi(int a, int b, fixed64 t)
{
   return a + ((b - a) * t);
}

struct vec2lk qbezierlk(fixed64 x1, fixed64 y1, fixed64 x2, fixed64 y2, fixed64 x3, fixed64 y3, fixed64 t)
{
   BezierImpl(fixed64, struct vec2lk, bzpolylk);
}

struct vec2i qbezieri(int x1, int y1, int x2, int y2, int x3, int y3, fixed64 t)
{
   BezierImpl(int, struct vec2i, bzpolyi);
}

struct polar ctopol(fixed x, fixed y)
{
   return (struct polar){ACS_VectorAngle(x, y), mag2i(x, y)};
}

// EOF
