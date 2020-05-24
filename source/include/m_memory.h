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

enum {
   _tag_free,
   _tag_head,
   _tag_libc,

   _tag_cybr,
   _tag_dlgs,
   _tag_file,
   _tag_huds,
   _tag_item,
   _tag_logs,
   _tag_plyr,
   _tag_ttle,

   _tag_max
};

#define Salloc(t, tag) Malloc(sizeof(t), tag)

stkcall void  Dalloc(void *p);
stkcall void *Malloc(size_t s, u32 tag);
stkcall void *Ralloc(void *p, size_t s, u32 tag);

#endif
