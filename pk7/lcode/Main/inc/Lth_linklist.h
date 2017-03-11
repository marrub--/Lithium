//-----------------------------------------------------------------------------
//
// Copyright © 2016-2017 Project Golan
//
// See "LICENSE.lithos3" for more information.
//
//-----------------------------------------------------------------------------
//
// Doubly linked list structure.
//
//-----------------------------------------------------------------------------

#ifndef lithos3__Lth_linklist_h
#define lithos3__Lth_linklist_h

#define Lth_ListForEach(tmpv, lst) \
   for(Lth_LinkList *list = (lst); list; list = list->next) \
      __with(tmpv = list->owner;)

#define Lth_ListFor(tmpv, lst) \
   for(Lth_LinkList *list = (lst); list;) \
      __with(Lth_LinkList *next = list->next, **prev = list->prev; \
         tmpv = list->owner;)

#define Lth_ListInsert(list, object, head) \
   (void) \
   ( \
      Lth_ListLink((list), (head)), \
      (list)->owner = object \
   )

#define Lth_ListInsertTail(list, object, head, tail) \
   (void) \
   ( \
      Lth_ListLinkTail((list), (head), (tail)), \
      (list)->owner = object \
   )

#define Lth_ListMove(list, head) \
   (void) \
   ( \
      Lth_ListRemove((list)), \
      Lth_ListLink((list), (head)) \
   )


// Type Definitions ----------------------------------------------------------|

//
// Lth_LinkList
//
typedef struct Lth_LinkList
{
   struct Lth_LinkList *next, **prev;
   void *owner;
} Lth_LinkList;


// Static Functions ----------------------------------------------------------|

//
// Lth_ListLink
//
static inline void Lth_ListLink(Lth_LinkList *list, Lth_LinkList **head)
{
   Lth_LinkList *next = *head;

   if((list->next = next))
      next->prev = &list->next;

   list->prev = head;
   *head = list;
}

//
// Lth_ListLinkTail
//
static inline void Lth_ListLinkTail(Lth_LinkList *list, Lth_LinkList **head,
   Lth_LinkList **tail)
{
   Lth_LinkList *prev = *tail;

   if(*(list->prev = head))
      list->prev = &prev->next;
   else
      *head = list;

   prev->next = list;
   *tail = list;
}

//
// Lth_ListRemove
//
static inline void Lth_ListRemove(Lth_LinkList *list)
{
   Lth_LinkList *next = list->next, **prev = list->prev;

   if(prev && (*prev = next))
      next->prev = prev;

   list->next = NULL;
   list->prev = NULL;
}

#endif//lithos3__Lth_linklist_h
