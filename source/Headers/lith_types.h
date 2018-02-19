// Copyright © 2018 Graham Sanderson, all rights reserved.
#ifndef lith_types_h
#define lith_types_h

#include <stdint.h>
#include <stdbool.h>
#include <stdfix.h>

typedef int64_t i64;
typedef int96_t i96;

typedef uint32_t u32;
typedef uint64_t u64;
typedef uint96_t u96;

typedef unsigned char byte;

typedef long fixed fixed64;

struct polar {
   fixed ang;
   fixed dst;
};

struct vec2f {
   float x, y;
};

struct vec2i {
   int x, y;
};

#endif
