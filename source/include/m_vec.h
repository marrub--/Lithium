/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Dynamically allocated arrays.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef m_vec_h
#define m_vec_h

#include "m_memory.h"
#include "m_types.h"

#include <stdlib.h>

#define Vec_Defn(type, name, ...) \
   __VA_ARGS__ type  *name##V = nil; \
   __VA_ARGS__ size_t name##C = 0, name##S = 0

#define Vec_Decl(type, name, ...) \
   __VA_ARGS__ type  *name##V; \
   __VA_ARGS__ size_t name##C, name##S

#define Vec_GrowV(vec, n, tag) \
   ((vec##S) += (n), \
    (vec##V) = Ralloc((vec##V), sizeof(*(vec##V)) * (vec##S), (tag)))

#define Vec_GrowN(vec, n, g, tag) \
   ((vec##C) + (n) > (vec##S) ? \
    Vec_GrowV(vec, (n) + (g), tag) : \
    (void *)0)

#define Vec_Grow(vec, n, tag) Vec_GrowN(vec, n, 8, tag)

#define Vec_Resize(vec, n, tag) \
   ((vec##C) = n, (vec##S) >= (vec##C) ? (void)0 : (void)Vec_GrowV(vec, n - (vec##S), tag))

#define Vec_Next(vec) ((vec##V)[(vec##C)++])

#define Vec_Clear(vec) \
   (Dalloc((vec##V)), (vec##V) = nil, (vec##C) = (vec##S) = 0)

#define Vec_MoveEnd(vec, i, l) (fastmemmove(&(vec##V)[(vec##C)], i, l), (vec##C) += (l))

#endif
