// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// m_memory.h: Memory allocation functions.

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
#define Calloc(n, s) (printf("%s:%i: Calloc\n", __FILE__, __LINE__), Calloc_real(n, s))
#define Dalloc(p)    (printf("%s:%i: Dalloc\n", __FILE__, __LINE__), Dalloc_real(p))
#define Nalloc(s)    (printf("%s:%i: Nalloc\n", __FILE__, __LINE__), Nalloc_real(s))
#define Malloc(s)    (printf("%s:%i: Malloc\n", __FILE__, __LINE__), Malloc_real(s))
#define Ralloc(p, s) (printf("%s:%i: Ralloc\n", __FILE__, __LINE__), Ralloc_real(p, s))
#define Salloc(t)    (printf("%s:%i: Salloc\n", __FILE__, __LINE__), Salloc_real(t))
#else
#define Calloc(n, s) Calloc_real(n, s)
#define Dalloc(p)    Dalloc_real(p)
#define Nalloc(s)    Nalloc_real(s)
#define Malloc(s)    Malloc_real(s)
#define Ralloc(p, s) Ralloc_real(p, s)
#define Salloc(t)    Salloc_real(t)
#endif

#endif
