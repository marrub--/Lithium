// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Various math functions.                                                  │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#if defined(abs_impl)
   [[return]] __asm(
      "BAnd    (Stk() LocReg(Lit(:n)) Lit(0x80000000_s31.0))"
      "Jcnd_Tru(Stk() Lit(:\"neg\"))"
      "Retn    (LocReg(Lit(:n)))"
   ":\"neg\""
      "Neg:I(Stk() LocReg(Lit(:n)))"
      "Retn (Stk())"
   );
#undef abs_impl
#elif defined(min_max_impl) && min_max_impl == 0
   return x < y ? x : y;
#undef min_max_impl
#elif defined(min_max_impl) && min_max_impl == 1
   return x < y ? y : x;
#undef min_max_impl
#elif defined(min_max_impl) && min_max_impl == 2
   return x < y ? y : x < z ? x : z;
#undef min_max_impl
#elif defined(crc64_impl_type)
   crc64_impl_type const *ptr = data;

   if(!crcinit) InitCRC64();

   result = ~result;

   for(mem_size_t i = 0; i < len; i++) {
      result = crctable[byte(result ^ ptr[i])] ^ (result >> 8);
   }

   return ~result;
#undef crc64_impl_type
#elif defined(bezier_impl_retn)
   noinit static
   bezier_impl_retn r;
   bezier_impl_type xa = bezier_impl_func(x1, x2, t);
   bezier_impl_type ya = bezier_impl_func(y1, y2, t);
   bezier_impl_type xb = bezier_impl_func(x2, x3, t);
   bezier_impl_type yb = bezier_impl_func(y2, y3, t);
   r.x = bezier_impl_func(xa, xb, t);
   r.y = bezier_impl_func(ya, yb, t);
   return r;
#undef bezier_impl_retn
#undef bezier_impl_type
#undef bezier_impl_func
#elif defined(int_lerp_impl_type)
   /* code by Kate, originally for Doom RPG. */
   if(data->value != data->value_old) {
      data->value_start = data->value_display;
      data->timer_max =
         llabs(data->value - data->value_display) * data->timer_max_cap;
      data->timer_max = mini(data->timer_max, data->timer_max_cap * 35);
      data->timer = data->timer_max - 1;
   }

   if(data->timer > 0) {
      int_lerp_impl_type timer_max2 = data->timer_max * data->timer_max;
      int_lerp_impl_type timer2     = data->timer     * data->timer;

      int_lerp_impl_type perc = timer_max2 - timer2;

      data->value_display =
         data->value_start + ((data->value - data->value_start) * perc /
                              timer_max2);

      data->timer--;
   } else {
      data->value_display = data->value;
   }

   data->value_old = data->value;
#undef int_lerp_impl_type
#elif defined(int_lerp_init_impl)
   data->value         = value;
   data->value_old     = value;
   data->value_start   = value;
   data->value_display = value;
   data->timer         = 0;
   data->timer_max     = 0;
   data->timer_max_cap = 2;
#undef int_lerp_init_impl
#else
#include "m_engine.h"

#include <math.h>

#define K32_LO 0x0000FFFFU
#define K32_HI 0xFFFF0000U
#define K64_LO 0x00000000FFFFFFFFUL
#define K64_HI 0xFFFFFFFF00000000UL

#define POLYNOMIAL 0xC96C5795D7870F42 /* ECMA 182 */

noinit union ik32 ik32;
noinit union ik64 ik64;

noinit static
u64 crctable[256]; /* NB: Don't try to hash >8bit data. */

noinit static
bool crcinit;

static
void InitCRC64(void) {
   for(u64 i = 0; i < 256; i++) {
      u64 remainder = i;

      for(i32 j = 0; j < 8; j++) {
         if(remainder & 1) remainder = (remainder >> 1) ^ POLYNOMIAL;
         else              remainder >>= 1;
      }

      crctable[i] = remainder;
   }

   crcinit = true;
}

u64 crc64(void const *data, mem_size_t len, u64 result) {
   #define crc64_impl_type mem_byte_t
   #include "m_math.c"
}

u64 crc64_str(void  __str_ars const *data, mem_size_t len, u64 result) {
   #define crc64_impl_type mem_byte_t __str_ars
   #include "m_math.c"
}

stkoff i32 rainbowcr(void) {
   static i32 crs[7];
   if(!crs[0]) {
      crs[0] = Cr(rred);
      crs[1] = Cr(rora);
      crs[2] = Cr(ryel);
      crs[3] = Cr(rgrn);
      crs[4] = Cr(rblu);
      crs[5] = Cr(rpur);
      crs[6] = Cr(rpnk);
   }
   return crs[ACS_Timer() / 4 % countof(crs)];
}

stkoff i32 const_deg(i32 n) {
   n %= 360;
   if(n < 0) n += 360;
   return n;
}

stkoff i32 fastabs(i32 n) {
   #define abs_impl
   #include "m_math.c"
}
stkoff i64 fastabsl(i64 n) {
   #define abs_impl
   #include "m_math.c"
}
stkoff i96 fastabsll(i96 n) {
   #define abs_impl
   #include "m_math.c"
}
stkoff k32 fastabsk(k32 n) {
   #define abs_impl
   #include "m_math.c"
}
stkoff i32 mini(i32 x, i32 y) {
   #define min_max_impl 0
   #include "m_math.c"
}
stkoff i64 minli(i64 x, i64 y) {
   #define min_max_impl 0
   #include "m_math.c"
}
stkoff u32 minu(u32 x, u32 y) {
   #define min_max_impl 0
   #include "m_math.c"
}
stkoff k32 mink(k32 x, k32 y) {
   #define min_max_impl 0
   #include "m_math.c"
}
stkoff k64 minlk(k64 x, k64 y) {
   #define min_max_impl 0
   #include "m_math.c"
}
stkoff i32 maxi(i32 x, i32 y) {
   #define min_max_impl 1
   #include "m_math.c"
}
stkoff i64 maxli(i64 x, i64 y) {
   #define min_max_impl 1
   #include "m_math.c"
}
stkoff u32 maxu(u32 x, u32 y) {
   #define min_max_impl 1
   #include "m_math.c"
}
stkoff k32 maxk(k32 x, k32 y) {
   #define min_max_impl 1
   #include "m_math.c"
}
stkoff k64 maxlk(k64 x, k64 y) {
   #define min_max_impl 1
   #include "m_math.c"
}
stkoff i32 clampi(i32 x, i32 y, i32 z) {
   #define min_max_impl 2
   #include "m_math.c"
}
stkoff i64 clampli(i64 x, i64 y, i64 z) {
   #define min_max_impl 2
   #include "m_math.c"
}
stkoff u32 clampu(u32 x, u32 y, u32 z) {
   #define min_max_impl 2
   #include "m_math.c"
}
stkoff k32 clampk(k32 x, k32 y, k32 z) {
   #define min_max_impl 2
   #include "m_math.c"
}
stkoff k64 clamplk(k64 x, k64 y, k64 z) {
   #define min_max_impl 2
   #include "m_math.c"
}

stkoff k32 fastfrack(k32 k) {
   ik32.k = k; ik32.i &= K32_LO;
   return ik32.k;
}

stkoff k64 fastfraclk(k64 k) {
   ik64.k = k; ik64.i &= K64_LO;
   return ik64.k;
}

stkoff
i32 fastroundk(k32 k) {
   ik32.k = k; ik32.i &= K32_LO;
   k64 fr = ik32.k;
   ik32.k = k; ik32.i &= K32_HI;
   if(fr > 0.5k) return ik32.k + 1.0k;
   else          return ik32.k;
}

stkoff
k32 fastround1k(k32 k) {
   ik32.k = k; ik32.i &= K32_LO;
   k32 fr = ik32.k;
   ik32.k = k; ik32.i &= K32_HI;
   /**/ if(fr > 0.9k) return ik32.i += 65536,  ik32.k;
   else if(fr > 0.8k) return ik32.i += 0xE667, ik32.k;
   else if(fr > 0.7k) return ik32.i += 0xCCCD, ik32.k;
   else if(fr > 0.6k) return ik32.i += 0xB334, ik32.k;
   else if(fr > 0.5k) return ik32.i += 0x999A, ik32.k;
   else if(fr > 0.4k) return ik32.i += 0x8001, ik32.k;
   else if(fr > 0.3k) return ik32.i += 0x6667, ik32.k;
   else if(fr > 0.2k) return ik32.i += 0x4CCD, ik32.k;
   else if(fr > 0.1k) return ik32.i += 0x3334, ik32.k;
   else if(fr > 0.0k) return ik32.i += 0x199A, ik32.k;
   else               return ik32.k;
}

stkoff
i32 fastroundlk(k64 k) {
   ik64.k = k; ik64.i &= K64_LO;
   k64 fr = ik64.k;
   ik64.k = k; ik64.i &= K64_HI;
   if(fr > 0.5lk) return ik64.k + 1.0lk;
   else           return ik64.k;
}

stkoff
k64 fastround1lk(k64 k) {
   ik64.k = k; ik64.i &= K64_LO;
   k64 fr = ik64.k;
   ik64.k = k; ik64.i &= K64_HI;
   /**/ if(fr > 0.9lk) return ik64.i += 4294967296, ik64.k;
   else if(fr > 0.8lk) return ik64.i += 0xE6666667, ik64.k;
   else if(fr > 0.7lk) return ik64.i += 0xCCCCCCCD, ik64.k;
   else if(fr > 0.6lk) return ik64.i += 0xB3333334, ik64.k;
   else if(fr > 0.5lk) return ik64.i += 0x9999999A, ik64.k;
   else if(fr > 0.4lk) return ik64.i += 0x80000001, ik64.k;
   else if(fr > 0.3lk) return ik64.i += 0x66666667, ik64.k;
   else if(fr > 0.2lk) return ik64.i += 0x4CCCCCCD, ik64.k;
   else if(fr > 0.1lk) return ik64.i += 0x33333334, ik64.k;
   else if(fr > 0.0lk) return ik64.i += 0x1999999A, ik64.k;
   else                return ik64.k;
}

stkoff
i32 ceilk(k32 n) {
   ik32.k = n;
   if(ik32.i & K32_LO) return ik32.i &= K32_HI, ik32.k + 1.0k;
   else                return ik32.k;
}

stkoff
k64 powlk(k64 x, i32 y) {
   k64 z = 1;
   while(y-- > 0) {z *= x;}
   return z;
}

stkoff
k32 mag2k(k32 x, k32 y) {
   return ACS_FixedSqrt(x * x + y * y);
}

stkoff
i32 mag2i(i32 x, i32 y) {
   return ACS_Sqrt(x * x + y * y);
}

stkoff k32 ease_in_out_back(k32 x) {
   #define C1 1.70158k
   #define C2 2.5949095k
   return x < 0.5k
      ? ((x * 2) * (x * 2) * ((C2 + 1) * 2 * x - C2)) / 2
      : ((x * 2 - 2) * (x * 2 - 2) * ((C2 + 1) * (x * 2 - 2) + C2) + 2) / 2;
   #undef C1
   #undef C2
}

stkoff k32 ease_out_cubic(k32 x) {
   x = 1 - x;
   return 1 - x * x * x;
}

stkoff k32 ease_out_sine(k32 x) {
   return ACS_Sin(x / 4);
}

stkoff k32 ease_in_out_sine(k32 x) {
   return -(ACS_Cos(x / 2) - 1) / 2;
}

stkoff k32 lerpk(k32 a, k32 b, k32 t) {
   return (1.0k - t) * a + t * b;
}

stkoff k64 lerplk(k64 a, k64 b, k64 t) {
   return (1.0lk - t) * a + t * b;
}

void lerpi(struct interp_data_i *data) {
   #define int_lerp_impl_type i32
   #include "m_math.c"
}

void lerpi_init(struct interp_data_i *data, i32 value, i32 timer) {
   #define int_lerp_init_impl
   #include "m_math.c"
}

void lerpli(struct interp_data_li *data) {
   #define int_lerp_impl_type i64
   #include "m_math.c"
}

void lerpli_init(struct interp_data_li *data, i64 value, i64 timer) {
   #define int_lerp_init_impl
   #include "m_math.c"
}

void lerplli(struct interp_data_lli *data) {
   #define int_lerp_impl_type i96
   #include "m_math.c"
}

void lerplli_init(struct interp_data_lli *data, i96 value, i96 timer) {
   #define int_lerp_init_impl
   #include "m_math.c"
}

stkoff bool aabb_aabb(i32 x1, i32 y1, i32 w1, i32 h1, i32 x2, i32 y2, i32 w2, i32 h2) {
   return
      x1 < x2 + h2 &&
      x1 + w1 > x2 &&
      y1 < y2 + h2 &&
      y1 + h1 > y2;
}

stkoff bool aabb_point(i32 x1, i32 y1, i32 w1, i32 h1, i32 x2, i32 y2) {
   return
      x2 >= x1 &&
      y2 >= y1 &&
      x2 <= x1 + w1 &&
      y2 <= y1 + h1;
}

stkoff k64 bzpolylk(k64 a, k64 b, k64 t) {
   return a + (b - a) * t;
}

stkoff i32 bzpolyi(i32 a, i32 b, k64 t) {
   return a + (b - a) * t;
}

struct k64v2 qbezierlk(k64 x1, k64 y1, k64 x2, k64 y2, k64 x3, k64 y3, k64 t) {
   #define bezier_impl_retn struct k64v2
   #define bezier_impl_type k64
   #define bezier_impl_func bzpolylk
   #include "m_math.c"
}

struct i32v2 qbezieri(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, k64 t) {
   #define bezier_impl_retn struct i32v2
   #define bezier_impl_type i32
   #define bezier_impl_func bzpolyi
   #include "m_math.c"
}

struct polar ctopol(k32 x, k32 y) {
   noinit static
   struct polar pol;

   pol.ang = atan2k(y, x);
   pol.dst = mag2i(x, y);

   return pol;
}

struct k32v3 trace_from(k32 yaw, k32 pitch, k32 dist, k32 offsetz, bool floor) {
   struct k32v3 v;
   ServCallV(sm_TraceFrom, yaw, pitch, dist, offsetz, floor,
             (intptr_t)&v.x, (intptr_t)&v.y, (intptr_t)&v.z);
   return v;
}

struct i32v2 project(k32 x, k32 y, k32 z, bool *visible) {
   struct i32v2 v;
   ServCallV(sm_Project, x, y, z,
             (intptr_t)&v.x, (intptr_t)&v.y, (intptr_t)visible);
   return v;
}
#endif
