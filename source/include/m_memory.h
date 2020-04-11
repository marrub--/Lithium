/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
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

#if MEMORY_DEBUG
#include <stdio.h>
#include <ACS_ZDoom.h>
#define AllocLog(name) (ACS_BeginPrint(), \
                        __nprintf("%s:%i: " name, __FILE__, __LINE__), \
                        ACS_EndLog())
#else
#define AllocLog(name) ((void)0)
#endif

#define Calloc(n, s) (AllocLog("Calloc"), Calloc_real(n, s))
#define Dalloc(p)    (AllocLog("Dalloc"), Dalloc_real(p))
#define Nalloc(s)    (AllocLog("Nalloc"), Nalloc_real(s))
#define Malloc(s)    (AllocLog("Malloc"), Malloc_real(s))
#define Ralloc(p, s) (AllocLog("Ralloc"), Ralloc_real(p, s))
#define Salloc(t)    (AllocLog("Salloc"), Salloc_real(t))

#endif
