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

#define for_list_none_it(head) \
   for(struct list *_head = head, *_rover = _head->next; \
       _rover != _head; Log("%s %p", __func__, _rover = _rover->next))

#define for_list_back_none_it(head) \
   for(struct list *_head = head, *_rover = _head->prev; \
       _rover != _head; _rover = _rover->prev)

#define for_list_it(var, head) \
   for_list_none_it(head) __with(var = _rover->object;)
#define for_list_back_it(var, head) \
   for_list_back_none_it(head) __with(var = _rover->object;)

#define for_list(var, head)           for_list_it(var, head)
#define for_list_back(var, head) for_list_back_it(var, head)

#define for_list_none(head)           for_list_none_it(head)
#define for_list_back_none(head) for_list_back_none_it(head)

/* Inline these. -Ten */
#define ListCtor(ls, obj) \
   statement({ \
      register struct list *_ls  = ls; \
      register void        *_obj = obj; \
      _ls->prev   = \
      _ls->next   = _ls; \
      _ls->object = _obj; \
   })

#define ListDtor(ls, ...) \
   statement({ \
      register struct list *_ls = ls; \
      while(_ls->next != _ls) { \
         register struct list *_rover = _ls->next; \
         ListUnlink(_rover); \
         statement(__VA_ARGS__); \
      } \
   })

#define ListLink(head, ls) \
   statement({ \
      register struct list *_head = head; \
      register struct list *_ls   = ls; \
      _ls->prev       = _head->prev; \
      _ls->next       = _head; \
      _ls->prev->next = \
      _ls->next->prev = _ls; \
   })

#define ListUnlink(ls) \
   statement({ \
      register struct list *_ls = ls; \
      _ls->prev->next = _ls->next; \
      _ls->next->prev = _ls->prev; \
      _ls->prev       = \
      _ls->next       = _ls; \
   })

/* Type Definitions -------------------------------------------------------- */

struct list {
   void *object;
   struct list *prev, *next;
};

#endif
