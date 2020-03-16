/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Memory allocation functions.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef m_memory_h
#define m_memory_h

#define Calloc_real(n, s) calloc(n, s)
#define Dalloc_real(p)    free(p)
#define Nalloc_real(s)    calloc(1, s)
#define Malloc_real(s)    malloc(s)
#define Ralloc_real(p, s) realloc(p, s)
#define Salloc_real(t)    calloc(1, sizeof(t))

#if LITH_MEMORY_DEBUG
#include <stdio.h>
#define Calloc(n, s) (Log("%s:%i: Calloc", __FILE__, __LINE__), Calloc_real(n, s))
#define Dalloc(p)    (Log("%s:%i: Dalloc", __FILE__, __LINE__), Dalloc_real(p))
#define Nalloc(s)    (Log("%s:%i: Nalloc", __FILE__, __LINE__), Nalloc_real(s))
#define Malloc(s)    (Log("%s:%i: Malloc", __FILE__, __LINE__), Malloc_real(s))
#define Ralloc(p, s) (Log("%s:%i: Ralloc", __FILE__, __LINE__), Ralloc_real(p, s))
#define Salloc(t)    (Log("%s:%i: Salloc", __FILE__, __LINE__), Salloc_real(t))
#else
#define Calloc(n, s) Calloc_real(n, s)
#define Dalloc(p)    Dalloc_real(p)
#define Nalloc(s)    Nalloc_real(s)
#define Malloc(s)    Malloc_real(s)
#define Ralloc(p, s) Ralloc_real(p, s)
#define Salloc(t)    Salloc_real(t)
#endif

#endif
