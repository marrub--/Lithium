/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Linked list functions.
 *
 * ---------------------------------------------------------------------------|
 */

#include "m_list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Extern Functions -------------------------------------------------------- */

void ListCtor(list *ls, void *object)
{
   ls->prev = ls->next = ls;
   ls->object = object;
}

void ListDtor(list *head, bool dofree)
{
   if(head->next)
   {
      while(head->next != head)
      {
         list *rover = head->next;
         rover->unlink();
         if(dofree)
            Dalloc(rover->object);
      }
   }
   else
      ListCtor(head);
}

void ListLink(list *head, list *ls)
{
   (ls->prev = head->prev)->next = ls;
   (ls->next = head      )->prev = ls;
}

void *ListUnlink(list *ls)
{
   ls->prev->next = ls->next;
   ls->next->prev = ls->prev;
   ls->prev = ls->next = ls;
   return ls->object;
}

size_t ListSize(list *head)
{
   size_t count = 0;
   for_list_none(*head) count++;
   return count;
}

/* EOF */
