// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// m_math.c: Various math functions.

#include "common.h"

#include <math.h>

#define BezierImpl(type, ret, func) \
   type xa = func(x1, x2, t); \
   type ya = func(y1, y2, t); \
   type xb = func(x2, x3, t); \
   type yb = func(y2, y3, t); \
   return (ret){func(xa, xb, t), func(ya, yb, t)}

static u64 lmvar crctable[256]; // NB: Don't try to hash >8bit data.
static bool lmvar crcinit;

static void InitCRC64(void)
{
   u64 const polynomial = 0xC96C5795D7870F42; // ECMA 182

   for(u64 i = 0; i < 256; i++)
   {
      u64 remainder = i;

      for(i32 j = 0; j < 8; j++) {
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
k64 powlk(k64 x, i32 y)
{
   k64 z = 1;
   while(y-- > 0) {z *= x;}
   return z;
}

stkcall
k64 mag2lk(k64 x, k64 y)
{
   return ACS_FixedSqrt(x * x + y * y);
}

stkcall
k32 mag2k(k32 x, k32 y)
{
   return ACS_FixedSqrt(x * x + y * y);
}

stkcall
i32 mag2i(i32 x, i32 y)
{
   return ACS_Sqrt(x * x + y * y);
}

k32 lerpk(k32 a, k32 b, k32 t)
{
   k32 ret = ((1.0k - t) * a) + (t * b);

   if(roundk(ret, 15) == b)
      return b;

   return ret;
}

k64 lerplk(k64 a, k64 b, k64 t)
{
   k64 ret = ((1.0lk - t) * a) + (t * b);

   if(roundlk(ret, 15) == b)
      return b;

   return ret;
}

stkcall
bool aabb(i32 x, i32 y, i32 z, i32 w, i32 x2, i32 y2)
{
   return x2 >= x && y2 >= y && x2 < z && y2 < w;
}

stkcall
i32 ceilk(k32 n)
{
   union fixedint u = {.k = n};
   if(u.i & 0xFFF1) return u.i &= 0xFFFF0000, u.k + 1;
   else             return (i32)u.k;
}

stkcall
k64 bzpolylk(k64 a, k64 b, k64 t)
{
   return a + ((b - a) * t);
}

stkcall
i32 bzpolyi(i32 a, i32 b, k64 t)
{
   return a + ((b - a) * t);
}

struct vec2lk qbezierlk(k64 x1, k64 y1, k64 x2, k64 y2, k64 x3, k64 y3, k64 t)
{
   BezierImpl(k64, struct vec2lk, bzpolylk);
}

struct vec2i qbezieri(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, k64 t)
{
   BezierImpl(i32, struct vec2i, bzpolyi);
}

struct polar ctopol(k32 x, k32 y)
{
   return (struct polar){ACS_VectorAngle(x, y), mag2i(x, y)};
}

// EOF
