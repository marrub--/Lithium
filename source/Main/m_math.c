// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_common.h"
#include <math.h>

#define BezierImpl(type, ret, func) \
   type xa = func(x1, x2, t); \
   type ya = func(y1, y2, t); \
   type xb = func(x2, x3, t); \
   type yb = func(y2, y3, t); \
   return (ret){func(xa, xb, t), func(ya, yb, t)}

static crc64_t lmvar crctable[256]; // NB: Don't try to hash >8bit data.
static bool lmvar crcinit;

//
// InitCRC64
//
static void InitCRC64()
{
   crc64_t const polynomial = 0xC96C5795D7870F42; // ECMA 182

   for(crc64_t i = 0; i < 256; i++)
   {
      crc64_t remainder = i;

      for(int j = 0; j < 8; j++) {
         if(remainder & 1) remainder = (remainder >> 1) ^ polynomial;
         else              remainder >>= 1;
      }

      crctable[i] = remainder;
   }

   crcinit = true;
}

//
// Lith_CRC64
//
crc64_t Lith_CRC64(void const *data, size_t len, crc64_t result)
{
   unsigned char const *ptr = data;

   if(!crcinit) InitCRC64();

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
   fixed ret = ((1.0k - t) * a) + (t * b);

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
   float ret = ((1.0f - t) * a) + (t * b);

   if((roundf(ret << 10) >> 10) == b)
      return b;

   return ret;
}

//
// lerp
//
double lerp(double a, double b, double t)
{
   #pragma GDCC FIXED_LITERAL OFF
   double ret = ((1.0 - t) * a) + (t * b);

   if((round(ret << 10) >> 10) == b)
      return b;

   return ret;
}

//
// aabb
//
bool aabb(int x, int y, int z, int w, int x2, int y2)
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

//
// bzpolyf
//
float bzpolyf(float a, float b, float t)
{
   return a + ((b - a) * t);
}

//
// bzpolyi
//
int bzpolyi(int a, int b, float t)
{
   return a + ((b - a) * t);
}

//
// qbezierf
//
struct vec2f qbezierf(float x1, float y1, float x2, float y2, float x3, float y3, float t)
{
   BezierImpl(float, struct vec2f, bzpolyf);
}

//
// qbezieri
//
struct vec2i qbezieri(int x1, int y1, int x2, int y2, int x3, int y3, float t)
{
   BezierImpl(int, struct vec2i, bzpolyi);
}

//
// ctopol
//
struct polar ctopol(fixed x, fixed y)
{
   return (struct polar){ACS_VectorAngle(x, y), mag2f(x, y)};
}

// EOF
