// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Math functions.                                                          │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef m_math_h
#define m_math_h

#define dst_bit(y)    (1 << (y))
#define get_bit(x, y) ((x) &   dst_bit(y))
#define set_bit(x, y) ((x) |=  dst_bit(y))
#define dis_bit(x, y) ((x) &= ~dst_bit(y))
#define tog_bit(x, y) ((x) ^=  dst_bit(y))

#define dst_msk(y)       (((1 << (y##_end - y##_beg)) - 1) << y##_beg)
#define val_msk(x, y)    (((x) <<  y##_beg)    &  dst_msk(y))
#define get_msk(x, y)    (((x) &   dst_msk(y)) >> y##_beg)
#define dis_msk(x, y)    ( (x) &= ~dst_msk(y))
#define set_msk(x, y, z) (dis_msk(x, y), (x) |= val_msk(z, y))

#define k32_to_i32(n) (ik32.k = (n), ik32.i)
#define i32_to_k32(n) (ik32.i = (n), ik32.k)

#define k32_to_byte(n) (byte(k32_to_i32((k32)(n) * 255.0) >> 16))

#define floork(n)  ((i32)(n))
#define floorlk(n) ((i64)(n))

#define word(n) ((n) & 0xFFFF)
#define byte(n) ((n) & 0xFF)

#define fourcc(a, b, c, d) ((d << 24) | (c << 16) | (b << 8) | (a << 0))

extern union ik32 ik32;
extern union ik64 ik64;

/* To make pitch values down=0, up=1 */
#define PITCH_BASE (-0.5)

/* π! */
#define pi  (3.14159265358979323846lk)
#define pi2 (pi / 2.0lk)
#define pi4 (pi / 4.0lk)
#define tau (pi * 2.0lk)

struct interp_data_i {
   i32 value;
   i32 value_old;
   i32 value_start;
   i32 value_display;

   i32 timer;
   i32 timer_max;
   i32 timer_max_cap;
};

struct interp_data_li {
   i64 value;
   i64 value_old;
   i64 value_start;
   i64 value_display;

   i64 timer;
   i64 timer_max;
   i64 timer_max_cap;
};

struct interp_data_lli {
   i96 value;
   i96 value_old;
   i96 value_start;
   i96 value_display;

   i96 timer;
   i96 timer_max;
   i96 timer_max_cap;
};

optargs(1) u64 crc64(void const *data, mem_size_t len, u64 result);
optargs(1) u64 crc64_str(void __str_ars const *data, mem_size_t len, u64 result);
stkcall i32 rainbowcr(void);
stkcall i32 const_deg(i32 n);
stkcall i32 fastabs(i32 n);
stkcall k32 fastabsk(k32 n);
stkcall i32 fastroundk(k32 k);
stkcall k32 fastround1k(k32 k);
stkcall i32 fastroundlk(k64 lk);
stkcall k64 fastround1lk(k64 lk);
stkcall i32 mini(i32 x, i32 y);
stkcall i64 minli(i64 x, i64 y);
stkcall u32 minu(u32 x, u32 y);
stkcall k32 mink(k32 x, k32 y);
stkcall k64 minlk(k64 x, k64 y);
stkcall i32 maxi(i32 x, i32 y);
stkcall i64 maxli(i64 x, i64 y);
stkcall u32 maxu(u32 x, u32 y);
stkcall k32 maxk(k32 x, k32 y);
stkcall k64 maxlk(k64 x, k64 y);
stkcall i32 clampi(i32 x, i32 y, i32 z);
stkcall i64 clampli(i64 x, i64 y, i64 z);
stkcall u32 clampu(u32 x, u32 y, u32 z);
stkcall k32 clampk(k32 x, k32 y, k32 z);
stkcall k64 clamplk(k64 x, k64 y, k64 z);
stkcall i32 ceilk(k32 n);
stkcall k64 powlk(k64 x, i32 y);
stkcall k32 mag2k(k32 x, k32 y);
stkcall i32 mag2i(i32 x, i32 y);
stkcall k32 ease_in_out_back(k32 x);
stkcall k32 ease_out_cubic(k32 x);
stkcall k32 lerpk(k32 a, k32 b, k32 t);
stkcall k64 lerplk(k64 a, k64 b, k64 t);
void lerpi(struct interp_data_i *data);
void lerpi_init(struct interp_data_i *data, i32 value, i32 timer);
void lerpli(struct interp_data_li *data);
void lerpli_init(struct interp_data_li *data, i64 value, i64 timer);
void lerplli(struct interp_data_lli *data);
void lerplli_init(struct interp_data_lli *data, i96 value, i96 timer);
stkcall bool aabb_aabb(i32 x1, i32 y1, i32 w1, i32 h1, i32 x2, i32 y2, i32 w2, i32 h2);
stkcall bool aabb_point(i32 x1, i32 y1, i32 w1, i32 h1, i32 x2, i32 y2);
stkcall k64 bzpolylk(k64 a, k64 b, k64 t);
stkcall i32 bzpolyi(i32 a, i32 b, k64 t);
struct i32v2 qbezieri(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, k64 t);
struct k64v2 qbezierlk(k64 x1, k64 y1, k64 x2, k64 y2, k64 x3, k64 y3, k64 t);
struct polar ctopol(k32 x, k32 y);
optargs(1) struct k32v3 trace_from(k32 yaw, k32 pitch, k32 dist, k32 offsetz, bool floor);
optargs(1) struct i32v2 project(k32 x, k32 y, k32 z, bool *visible);

#endif
