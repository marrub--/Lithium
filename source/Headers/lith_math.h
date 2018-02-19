// Copyright © 2018 Graham Sanderson, all rights reserved.

#define max(x, y) ((x) < (y) ? (y) : (x))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define minmax(x, mi, ma) (min(max(x, mi), ma))

// To make pitch values down=0, up=1
#define PITCH_BASE (-0.5)

// π!
#define pi  (3.14159265358979323846f)
#define pi2 (pi / 2.0f)
#define pi4 (pi / 4.0f)
#define tau (pi * 2.0f)

[[__optional_args(1)]] u64 Lith_CRC64(void const *data, size_t len, u64 result);
[[__optional_args(1)]] u64 Lith_CRC64_str(void __str_ars const *data, size_t len, u64 result);
[[__optional_args(1)]] float RandomFloat(float max, float min);
float mag2f(float x, float y);
[[__call("StkCall")]] fixed mag2k(fixed x, fixed y);
[[__call("StkCall")]] int mag2i(int x, int y);
fixed lerpk(fixed a, fixed b, fixed t);
fixed64 lerplk(fixed64 a, fixed64 b, fixed64 t);
float lerpf(float a, float b, float t);
double lerp(double a, double b, double t);
[[__call("StkCall")]] bool aabb(int x, int y, int z, int w, int x2, int y2);
[[__call("StkCall")]] int ceilk(fixed n);
[[__call("StkCall")]] float bzpolyf(float a, float b, float t);
[[__call("StkCall")]] int bzpolyi(int a, int b, float t);
struct vec2i qbezieri(int x1, int y1, int x2, int y2, int x3, int y3, float t);
struct vec2f qbezierf(float x1, float y1, float x2, float y2, float x3, float y3, float t);
struct polar ctopol(fixed x, fixed y);

// EOF
