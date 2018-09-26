// Copyright © 2018 Graham Sanderson, all rights reserved.
#ifndef lith_types_h
#define lith_types_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define fixed
#include <stdfix.h>
#undef fixed

#define null NULL

#define stkcall [[__call("StkCall")]]
#define script [[__call("ScriptS")]]
#define optargs(x) [[__optional_args(x)]]
#define noinit [[__no_init]]
#define anonymous [[__anonymous]]
#define ext(x) [[__extern(x)]]
#define stksize(x) [[__alloc_Aut(x)]]
#define type(x) [[__script(x)]]
#define addr(x) [[__address(x)]]

typedef int64_t i64;
typedef int96_t i96;

typedef uint32_t u32;
typedef uint64_t u64;
typedef uint96_t u96;

typedef unsigned char byte;

typedef      _Accum fixed;
typedef long _Accum fixed64;

struct polar {
   fixed ang;
   fixed dst;
};

struct vec2lk {
   fixed64 x, y;
};

struct vec2i {
   int x, y;
};

#endif
