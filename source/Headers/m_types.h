// Copyright © 2018 Alison Sanderson, all rights reserved.
#ifndef m_types_h
#define m_types_h

#include <stdbool.h>
#include <stddef.h>
#include <stdfix.h>
#include <stdint.h>

#define nil NULL

#define stkcall    [[__call("StkCall")]]
#define script     [[__call("ScriptS")]]
#define optargs(x) [[__optional_args(x)]]
#define noinit     [[__no_init]]
#define anonymous  [[__anonymous]]
#define ext(x)     [[__extern(x)]]
#define stksize(x) [[__alloc_Aut(x)]]
#define type(x)    [[__script(x)]]
#define addr(x)    [[__address(x)]]

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

struct polar    {k32 ang, dst;};
struct vec2lk   {k64 x, y;};
struct vec2i    {i32 x, y;};
union  fixedint {int_k_t i; k32 k;};

#endif
