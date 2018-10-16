// Copyright © 2017 Alison Sanderson, all rights reserved.
#ifndef lith_darray_h
#define lith_darray_h

#include "lith_memory.h"

#include <stdlib.h>

#define Vec_Defn(type, name, ...) \
   __VA_ARGS__ type  *name##V = NULL; \
   __VA_ARGS__ size_t name##C = 0, name##S = 0

#define Vec_Decl(type, name, ...) \
   __VA_ARGS__ type  *name##V; \
   __VA_ARGS__ size_t name##C, name##S

#define Vec_GrowN(vec, n, g) \
   ((vec##C) + (n) > (vec##S) ? \
      ((vec##S) += (n) + (g), \
       (vec##V) = Ralloc((vec##V), sizeof(*(vec##V)) * (vec##S))) : \
      (void *)0)

#define Vec_Grow(vec, n) Vec_GrowN(vec, n, 8)

#define Vec_Next(vec) ((vec##V)[(vec##C)++])

#define Vec_Clear(vec) \
   (Dalloc((vec##V)), (vec##V) = NULL, (vec##C) = (vec##S) = 0)

#endif
