// Copyright © 2016-2018 Alison Sanderson, all rights reserved.
#include "m_list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Extern Functions ----------------------------------------------------------|

void Lith_LinkDefault(list *ls, void *object)
{
   ls->prev = ls->next = ls;
   ls->object = object;
}

void Lith_ListLink(list *head, list *ls)
{
   (ls->prev = head->prev)->next = ls;
   (ls->next = head      )->prev = ls;
}

void *Lith_ListUnlink(list *ls)
{
   ls->prev->next = ls->next;
   ls->next->prev = ls->prev;
   ls->prev = ls->next = ls;
   return ls->object;
}

size_t Lith_ListSize(list *head)
{
   size_t count = 0;
   for_list_none(*head) count++;
   return count;
}

void Lith_ListFree(list *head, bool dofree)
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
      head->construct();
}

// EOF
