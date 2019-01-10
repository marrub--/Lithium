// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// m_types.h: Common type definitions and attribute macros.

#ifndef m_types_h
#define m_types_h

#include <stdbool.h>
#include <stddef.h>
#include <stdfix.h>
#include <stdint.h>

#define nil  ((void *)0)
#define snil ((__str_ent *)0)
#define lnil ((__label *)0)

#define stkcall    [[__call("StkCall")]]
#define script     [[__call("ScriptI")]]
#define script_str [[__call("ScriptS")]]
#define optargs(x) [[__optional_args(x)]]
#define noinit     [[__no_init]]
#define anonymous  [[__anonymous]]
#define ext(x)     [[__extern(x)]]
#define stksize(x) [[__alloc_Aut(x)]]
#define type(x)    [[__script(x)]]
#define addr(x)    [[__address(x)]]

// Types ---------------------------------------------------------------------|

typedef int32_t i32;
typedef int64_t i64;
typedef int96_t i96;

typedef uint32_t u32;
typedef uint64_t u64;
typedef uint96_t u96;

typedef unsigned char byte;

typedef __str str;

typedef      _Accum k32;
typedef long _Accum k64;

typedef float  f32;
typedef double f64;

struct polar {k32 ang, dst;};
struct k64v2 {k64 x, y;};
struct i32v2 {i32 x, y;};

union ik32 {int_k_t i; k32 k;};

// Address Space Definitions -------------------------------------------------|

__addrdef extern __mod_arr lmvar;
__addrdef extern __hub_arr lhvar;

#endif
