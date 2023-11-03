// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Common type definitions and attribute macros.                            │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef m_types_h
#define m_types_h

#pragma GDCC FIXED_LITERAL ON

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdfix.h>
#include <stdint.h>

#define countof(a) (sizeof(a) / sizeof(*(a)))
#define swap(t, a, b) statement({ t _tmp = a; a = b; b = _tmp; })

#define nil  ((void *)0)
#define snil ((__str_ent *)0)
#define lnil ((__label *)0)

#define script          [[__call("ScriptI")]]
#define script_str      [[__call("ScriptS")]]
#define script_sync     [[__call("SScriptI")]]
#define script_sync_str [[__call("SScriptS")]]
#define stkcall      [[__call("StkCall")]]
#define stkoff       alloc_aut(0) stkcall
#define optargs(x)   [[__optional_args(x)]]
#define noinit       [[__no_init]]
#define anonymous    [[__anonymous]]
#define ext(x)       [[__extern(x)]]
#define alloc_aut(x) [[__alloc_Aut(x)]]
#define dynam_aut    [[__alloc_Aut(0x80000000)]]
#define type(x)      [[__script(x)]]
#define addr(x)      [[__address(x)]]

/* makes it easier to find function typedefs -zoe */
#define funcdef typedef

typedef int32_t i32;
typedef int64_t i64;
typedef int96_t i96;

typedef uint32_t u32;
typedef uint64_t u64;
typedef uint96_t u96;

#define SCR_MAX         INT64_MAX
#define FMT_SCR         "li"
#define faststrtoscr    faststrtoi64
#define lerpscr         lerpli
#define lerpscr_init    lerpli_init
#define interp_data_scr interp_data_li
#define minscr          minli
#define maxscr          maxli
#define clampscr        clampli

#define _fmt_scr  _fmt_i64
#define _fmt_time _fmt_u96

#define fmt_scr_val  li
#define fmt_time_val ulli

typedef i64 score_t;
typedef u96 time_t;

typedef i32 mem_tag_t;
typedef i32 mem_size_t;
typedef i32 mem_byte_t;

typedef __label   *lbl;
typedef __str_ent *str;

typedef char __str_ars const *astr;
typedef char           const *cstr;

typedef      _Accum k32;
typedef long _Accum k64;

typedef float  f32;
typedef double f64;

typedef                    __div_t i32div;
typedef               long __div_t i64div;
typedef          long long __div_t i96div;
typedef unsigned           __div_t u32div;
typedef unsigned      long __div_t u64div;
typedef unsigned long long __div_t u96div;
typedef               long __div_t scorediv;
typedef unsigned long long __div_t timediv;

struct polar {k32 ang, dst;};
struct i32v2 {i32 x, y;};
struct k32v2 {k32 x, y;};
struct k64v2 {k64 x, y;};
struct i32v3 {i32 x, y, z;};
struct k32v3 {k32 x, y, z;};
struct k64v3 {k64 x, y, z;};
struct i32v4 {i32 x, y, z, w;};
struct k32v4 {k32 x, y, z, w;};
struct k64v4 {k64 x, y, z, w;};
struct ptr2  {mem_size_t l, h;};

union ik32 {i32 i; k32 k;};
union ik64 {i64 i; k64 k;};

__addrdef extern __mod_arr lmvar;

#endif
