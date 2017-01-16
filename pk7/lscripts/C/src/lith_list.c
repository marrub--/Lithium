#include "lith_list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//----------------------------------------------------------------------------
// Extern Functions
//

void Lith_LinkDefault(list_t *list, void *object)
{
   list->prev = list->next = list;
   list->object = object;
}

void Lith_ListLink(list_t *head, list_t *list)
{
   (list->prev = head->prev)->next = list;
   (list->next = head      )->prev = list;
}

void Lith_ListUnlink(list_t *list, list_deleter_t deleter)
{
   list->prev->next = list->next;
   list->next->prev = list->prev;
   list->prev = list->next = list;
   
   if(deleter) deleter(list->object);
}

size_t Lith_ListSize(list_t *head)
{
   size_t count = 0;
   for(list_t *rover = head->next; rover != head; rover = rover->next)
      count++;
   return count;
}

void Lith_ListFree(list_t *list, list_deleter_t deleter)
{
   if(list->next == null)
      Lith_LinkDefault(list);
   else while(list->next->object)
   {
      list_t *ptr = list->next;
      Lith_ListUnlink(ptr);
      if(deleter) deleter(ptr->object);
   }
}

// EOF

