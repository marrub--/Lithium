/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Linked lists.
 *
 * ---------------------------------------------------------------------------|
 */

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

/* Extern Functions -------------------------------------------------------- */

optargs(1) void ListCtor(struct list *list, void *object);
optargs(1) void ListDtor(struct list *head, bool dofree);
void ListLink(struct list *head, struct list *list);
void *ListUnlink(struct list *list);
size_t ListSize(struct list *head);

/* Type Definitions -------------------------------------------------------- */

struct list
{
   __prop link   {operator(): ListLink(..., this)}
   __prop unlink {operator(): ListUnlink(this)}
   __prop size   {operator(): ListSize(this)}

   void *object;
   struct list *prev, *next;
};

typedef struct list list;

#endif
