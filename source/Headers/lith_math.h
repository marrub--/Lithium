// Copyright © 2018 Graham Sanderson, all rights reserved.

#define max(x, y) ((x) < (y) ? (y) : (x))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define minmax(x, mi, ma) (min(max(x, mi), ma))

// To make pitch values down=0, up=1
#define PITCH_BASE (-0.5)

// π!
#define pi  (3.14159265358979323846lk)
#define pi2 (pi / 2.0lk)
#define pi4 (pi / 4.0lk)
#define tau (pi * 2.0lk)

optargs(1) u64 crc64(void const *data, size_t len, u64 result);
optargs(1) u64 crc64_str(void __str_ars const *data, size_t len, u64 result);
stkcall fixed64 powlk(fixed64 x, int y);
stkcall fixed64 mag2lk(fixed64 x, fixed64 y);
stkcall fixed mag2k(fixed x, fixed y);
stkcall int mag2i(int x, int y);
fixed lerpk(fixed a, fixed b, fixed t);
fixed64 lerplk(fixed64 a, fixed64 b, fixed64 t);
stkcall bool aabb(int x, int y, int z, int w, int x2, int y2);
stkcall int ceilk(fixed n);
stkcall fixed64 bzpolylk(fixed64 a, fixed64 b, fixed64 t);
stkcall int bzpolyi(int a, int b, fixed64 t);
struct vec2i qbezieri(int x1, int y1, int x2, int y2, int x3, int y3, fixed64 t);
struct vec2lk qbezierlk(fixed64 x1, fixed64 y1, fixed64 x2, fixed64 y2, fixed64 x3, fixed64 y3, fixed64 t);
struct polar ctopol(fixed x, fixed y);

// EOF
