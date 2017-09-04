// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#ifndef LITH_LIST_H
#define LITH_LIST_H

#include "lith_common.h"

#include <stddef.h>

#define Lith_ForList(var, name) \
   for(list_t *rover = (name).next; rover && rover != &(name); rover = rover->next) \
      __with(var = rover->object;)

#define Lith_ForListIter(var, name, ...) \
   for(list_t *rover = (name).next; rover && rover != &(name); rover = rover->next, __VA_ARGS__) \
      __with(var = rover->object;)

// Extern Functions ----------------------------------------------------------|

[[__optional_args(1)]] void Lith_LinkDefault(struct list_s *list, void *object);
void Lith_ListLink(struct list_s *head, struct list_s *list);
void *Lith_ListUnlink(struct list_s *list);
size_t Lith_ListSize(struct list_s *head);
[[__optional_args(1)]] void Lith_ListFree(struct list_s *head, void (*deleter)(void *));

// Type Definitions ----------------------------------------------------------|

// To clarify how these new linked list structures work:
// You put a list_t in your structure (or anywhere, you could allocate one)
// and run LinkDefault on it to construct it proper.
//
// You can then have a head node which you iterate over. The head itself is
// a dummy node with no object, but you can iterate over it bidirectionally,
// as the back is head->prev and the front is head->next.
//
// This removes any real need to reverse a linked list, and you can insert
// backwards. Just be careful that all the nodes get constructed correctly.
//
// You can delete an entire list (destroying objects with a custom function or
// not at all) with ListFree, and get the size of a headed list with ListSize.

typedef struct list_s
{
   __prop construct {call: Lith_LinkDefault(this)}
   __prop link      {call: Lith_ListLink(__arg, this)}
   __prop unlink    {call: Lith_ListUnlink(this)}
   __prop size      {get:  Lith_ListSize(this)}
   __prop free      {call: Lith_ListFree(this)}

   void *object;
   struct list_s *prev, *next;
} list_t;

#endif

