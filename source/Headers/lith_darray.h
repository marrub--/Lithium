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

#define Vec_GrowV(vec, n) \
   ((vec##S) += (n), \
    (vec##V) = Ralloc((vec##V), sizeof(*(vec##V)) * (vec##S)))

#define Vec_GrowN(vec, n, g) \
   ((vec##C) + (n) > (vec##S) ? \
    Vec_GrowV(vec, (n) + (g)) : \
    (void *)0)

#define Vec_Grow(vec, n) Vec_GrowN(vec, n, 8)

#define Vec_Resize(vec, n) \
   ((vec##C) = n, (vec##S) >= (vec##C) ? (void)0 : (void)Vec_GrowV(vec, n - (vec##S)))

#define Vec_Next(vec) ((vec##V)[(vec##C)++])

#define Vec_Clear(vec) \
   (Dalloc((vec##V)), (vec##V) = NULL, (vec##C) = (vec##S) = 0)

#define Vec_MoveEnd(vec, i, l) (memmove(&(vec##V)[(vec##C)], i, l), (vec##C) += (l))

#endif
