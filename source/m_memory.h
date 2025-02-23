// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Memory allocation functions.                                             │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#ifndef m_memory_h
#define m_memory_h

#include "m_types.h"

#include <stddef.h>

enum {
   _tag_free,
   _tag_head,
   _tag_libc,

   _tag_dlgv,
   _tag_dlgc,
   _tag_file,
   _tag_item,
   _tag_logs,
   _tag_plyr,

   _tag_max
};

#define Salloc(t, tag) Malloc(sizeof(t), tag)
#define Talloc(p, l, tag) Ralloc(p, sizeof((p)[0]) * (l), tag)

stkcall void  Dalloc(void *p);
stkcall void *Malloc(mem_size_t s, mem_tag_t tag);
stkcall void *Ralloc(void *p, mem_size_t s, mem_tag_t tag);
stkcall void  Xalloc(mem_tag_t tag);

#endif
