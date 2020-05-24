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

#include "m_types.h"

#include <stddef.h>

#define Salloc(t) Malloc(sizeof(t))

stkcall void  Dalloc(void *p);
stkcall void *Malloc(size_t s);
stkcall void *Ralloc(void *p, size_t s);

#endif
