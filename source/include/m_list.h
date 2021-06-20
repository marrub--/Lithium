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

#define for_list(var_, head_) \
   for(struct list *_rover = (head_); _rover; _rover = _rover->next) \
      __with(var_ = _rover->data;)

/* Inline these. -Ten
 * now make it work nicely! -Zoe
 */
#define ListDestroy(head_, ...) \
   statement({ \
      for(register struct list **_head = (head_); *_head;) { \
         register void *_obj = (*_head)->data; \
         ListRemove(*_head); \
         if(_obj) statement(__VA_ARGS__); \
      } \
   })

#define ListInsert(head_, cur_, data_) \
   statement({ \
      register struct list **_head = head_; \
      register struct list  *_cur  = cur_; \
      register struct list  *_next = *_head; \
      _cur->data = data_; \
      if((_cur->next = _next)) _next->prev = &_cur->next; \
      _cur->prev = _head; \
      *_head = _cur; \
   })

#define ListRemove(cur_) \
   statement({ \
      register struct list  *_cur  = cur_; \
      register struct list **_prev = _cur->prev; \
      register struct list  *_next = _cur->next; \
      if(_prev && (*_prev = _next)) _next->prev = _prev; \
      _cur->prev = nil; \
      _cur->next = nil; \
   })

/* Type Definitions -------------------------------------------------------- */

struct list {
   struct list **prev, *next;
   void        *data;
};

#endif
