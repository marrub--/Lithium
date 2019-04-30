/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Math functions.
 *
 * ---------------------------------------------------------------------------|
 */

#define max(x, y) ((x) < (y) ? (y) : (x))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define minmax(x, mi, ma) (min(max(x, mi), ma))

/* To make pitch values down=0, up=1 */
#define PITCH_BASE (-0.5)

/* π! */
#define pi  (3.14159265358979323846lk)
#define pi2 (pi / 2.0lk)
#define pi4 (pi / 4.0lk)
#define tau (pi * 2.0lk)

optargs(1) u64 crc64(void const *data, size_t len, u64 result);
optargs(1) u64 crc64_str(void __str_ars const *data, size_t len, u64 result);
stkcall i32 fastabs(i32 n);
stkcall k64 powlk(k64 x, i32 y);
stkcall k64 mag2lk(k64 x, k64 y);
stkcall k32 mag2k(k32 x, k32 y);
stkcall i32 mag2i(i32 x, i32 y);
k32 lerpk(k32 a, k32 b, k32 t);
k64 lerplk(k64 a, k64 b, k64 t);
stkcall bool aabb(i32 x, i32 y, i32 z, i32 w, i32 x2, i32 y2);
stkcall i32 ceilk(k32 n);
stkcall k64 bzpolylk(k64 a, k64 b, k64 t);
stkcall i32 bzpolyi(i32 a, i32 b, k64 t);
struct i32v2 qbezieri(i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, k64 t);
struct k64v2 qbezierlk(k64 x1, k64 y1, k64 x2, k64 y2, k64 x3, k64 y3, k64 t);
struct polar ctopol(k32 x, k32 y);

/* EOF */
