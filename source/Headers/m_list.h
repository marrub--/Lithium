// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// m_list.h: Linked lists.

#ifndef m_list_h
#define m_list_h

#include "common.h"

#include <stddef.h>

#define for_list_none_it(name, ...) \
   for(list *rover = (name).next, *nextp; rover && rover != &(name); rover = nextp, __VA_ARGS__) \
      __with(nextp = rover->next;)

#define for_list_back_none_it(name, ...) \
   for(list *rover = (name).prev, *nextp; rover && rover != &(name); rover = nextp, __VA_ARGS__) \
      __with(nextp = rover->prev;)

#define for_list_it(var, name, ...) \
   for_list_none_it(name, __VA_ARGS__) __with(var = rover->object;)
#define for_list_back_it(var, name, ...) \
   for_list_back_none_it(name, __VA_ARGS__) __with(var = rover->object;)

#define for_list(var, name)           for_list_it(var, name, (void)0)
#define for_list_back(var, name) for_list_back_it(var, name, (void)0)

#define for_list_none(name)           for_list_none_it(name, (void)0)
#define for_list_back_none(name) for_list_back_none_it(name, (void)0)

// Extern Functions ----------------------------------------------------------|

optargs(1) void Lith_LinkDefault(struct list *list, void *object);
void Lith_ListLink(struct list *head, struct list *list);
void *Lith_ListUnlink(struct list *list);
size_t Lith_ListSize(struct list *head);
optargs(1) void Lith_ListFree(struct list *head, bool dofree);

// Type Definitions ----------------------------------------------------------|

struct list
{
   __prop construct {call: Lith_LinkDefault(this)}
   __prop link      {call: Lith_ListLink(__arg, this)}
   __prop unlink    {call: Lith_ListUnlink(this)}
   __prop size      {call: Lith_ListSize(this)}
   __prop free      {call: Lith_ListFree(this)}

   void *object;
   struct list *prev, *next;
};

typedef struct list list;

#endif
