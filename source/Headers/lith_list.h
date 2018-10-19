// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#ifndef LITH_LIST_H
#define LITH_LIST_H

#include "lith_common.h"

#include <stddef.h>

#define forlist(var, name) \
   for(list_t *rover = (name).next, *nextp; rover && rover != &(name); rover = nextp) \
      __with(nextp = rover->next; var = rover->object;)

#define forlistIt(var, name, ...) \
   for(list_t *rover = (name).next; rover && rover != &(name); rover = rover->next, __VA_ARGS__) \
      __with(var = rover->object;)

// Extern Functions ----------------------------------------------------------|

optargs(1) void Lith_LinkDefault(struct list_s *list, void *object);
void Lith_ListLink(struct list_s *head, struct list_s *list);
void *Lith_ListUnlink(struct list_s *list);
size_t Lith_ListSize(struct list_s *head);
optargs(1) void Lith_ListFree(struct list_s *head, bool dofree);

// Type Definitions ----------------------------------------------------------|

typedef struct list_s
{
   __prop construct {call: Lith_LinkDefault(this)}
   __prop link      {call: Lith_ListLink(__arg, this)}
   __prop unlink    {call: Lith_ListUnlink(this)}
   __prop size      {call: Lith_ListSize(this)}
   __prop free      {call: Lith_ListFree(this)}

   void *object;
   struct list_s *prev, *next;
} list_t;

#endif

