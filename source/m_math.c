/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Various math functions.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"

#include <math.h>

#define BezierImpl(type, ret, func) \
   static ret r; \
   type xa = func(x1, x2, t); \
   type ya = func(y1, y2, t); \
   type xb = func(x2, x3, t); \
   type yb = func(y2, y3, t); \
   r.x = func(xa, xb, t); \
   r.y = func(ya, yb, t); \
   return r

union ik32 ik32;
union uk32 uk32;

static u64 lmvar crctable[256]; /* NB: Don't try to hash >8bit data. */
static bool lmvar crcinit;

static void InitCRC64(void)
{
   u64 const polynomial = 0xC96C5795D7870F42; /* ECMA 182 */

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

alloc_aut(0) stkcall
i32 fastabs(i32 n)
{
   [[return]] __asm(
      "BAnd    (Stk() LocReg(Lit(:n)) Lit(0x80000000_s31.0))"
      "Jcnd_Tru(Stk() Lit(:\"neg\"))"
      "Retn    (LocReg(Lit(:n)))"
   ":\"neg\""
      "Neg:I(Stk() LocReg(Lit(:n)))"
      "Retn (Stk())"
   );
}

alloc_aut(0) stkcall
k32 fastabsk(k32 n)
{
   [[return]] __asm(
      "BAnd    (Stk() LocReg(Lit(:n)) Lit(0x80000000_s31.0))"
      "Jcnd_Tru(Stk() Lit(:\"neg\"))"
      "Retn    (LocReg(Lit(:n)))"
   ":\"neg\""
      "Neg:I(Stk() LocReg(Lit(:n)))"
      "Retn (Stk())"
   );
}

alloc_aut(0) stkcall
k32 fastroundk(k32 k, i32 n) {
   i32 mask = ~(0xFFFF >> n);

   ik32.k = k;

   if(ik32.i & (((0xFFFF >> 1) + 1) >> n)) {
      if((ik32.i & mask) == (mask & 0x7FFFFFFF))
         return ACCUM_MAX;

      ik32.i += (0xFFFF + 1) >> n;
   }

   ik32.i &= mask;

   return ik32.k;
}

alloc_aut(0) stkcall
k64 powlk(k64 x, i32 y)
{
   k64 z = 1;
   while(y-- > 0) {z *= x;}
   return z;
}

alloc_aut(0) stkcall
k64 mag2lk(k64 x, k64 y)
{
   return ACS_FixedSqrt(x * x + y * y);
}

alloc_aut(0) stkcall
k32 mag2k(k32 x, k32 y)
{
   return ACS_FixedSqrt(x * x + y * y);
}

alloc_aut(0) stkcall
i32 mag2i(i32 x, i32 y)
{
   return ACS_Sqrt(x * x + y * y);
}

alloc_aut(0) stkcall
k32 lerpk(k32 a, k32 b, k32 t)
{
   k32 ret = (1.0k - t) * a + t * b;

   if(fastroundk(ret, 15) == b)
      return b;

   return ret;
}

k64 lerplk(k64 a, k64 b, k64 t)
{
   k64 ret = (1.0lk - t) * a + t * b;

   if(roundlk(ret, 15) == b)
      return b;

   return ret;
}

/* code by Kate, originally for Doom RPG. */
void lerplli(struct interp_data_lli *data)
{
   if(data->value != data->value_old) {
      data->value_start = data->value_display;
      data->timer_max =
         llabs(data->value - data->value_display) * data->timer_max_cap;
      data->timer_max = min(data->timer_max, data->timer_max_cap * 35);
      data->timer = data->timer_max - 1;
   }

   if(data->timer > 0) {
      i96 timer_max2 = data->timer_max * data->timer_max;
      i96 timer2     = data->timer     * data->timer;

      i96 perc = timer_max2 - timer2;

      data->value_display =
         data->value_start + ((data->value - data->value_start) * perc /
                              timer_max2);

      data->timer--;
   } else {
      data->value_display = data->value;
   }

   data->value_old = data->value;
}

void lerplli_init(struct interp_data_lli *data, i96 value, i96 timer)
{
   data->value         = value;
   data->value_old     = value;
   data->value_start   = value;
   data->value_display = value;
   data->timer         = 0;
   data->timer_max     = 0;
   data->timer_max_cap = 2;
}

alloc_aut(0) stkcall
bool aabb_aabb(i32 x1, i32 y1, i32 w1, i32 h1, i32 x2, i32 y2, i32 w2, i32 h2)
{
   return
      x1 < x2 + h2 &&
      x1 + w1 > x2 &&
      y1 < y2 + h2 &&
      y1 + h1 > y2;
}

alloc_aut(0) stkcall
bool aabb_point(i32 x1, i32 y1, i32 w1, i32 h1, i32 x2, i32 y2) {
   return
      x2 >= x1 &&
      y2 >= y1 &&
      x2 <= x1 + w1 &&
      y2 <= y1 + h1;
}

alloc_aut(0) stkcall
i32 ceilk(k32 n)
{
   ik32.k = n;
   if(ik32.i & 0xFFF1) return ik32.i &= 0xFFFF0000, ik32.k + 1;
   else                return ik32.k;
}

alloc_aut(0) stkcall
k64 bzpolylk(k64 a, k64 b, k64 t)
{
   return a + (b - a) * t;
}

alloc_aut(0) stkcall
i32 bzpolyi(i32 a, i32 b, k64 t)
{
   return a + (b - a) * t;
}

struct k64v2 qbezierlk(k64 x1, k64 y1, k64 x2, k64 y2, k64 x3, k64 y3, k64 t)
{
   BezierImpl(k64, struct k64v2, bzpolylk);
}

struct i32v2 qbezieri(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, k64 t)
{
   BezierImpl(i32, struct i32v2, bzpolyi);
}

struct polar ctopol(k32 x, k32 y)
{
   static struct polar pol;
   pol.ang = ACS_VectorAngle(x, y);
   pol.dst = mag2i(x, y);
   return pol;
}

/* EOF */
