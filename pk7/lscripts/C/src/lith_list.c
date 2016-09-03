#include "lith_list.h"
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Doubly-linked lists.
//

slist_t *SList_Create(listdata_t data)
{
   slist_t *node = malloc(sizeof(slist_t));
   node->data = data;
   node->next = null;
   node->prev = null;
   return node;
}

slist_t *SList_InsertNext(slist_t *list, listdata_t data)
{
   if(list->next)
   {
      slist_t *rover = list->next;
      list->next = SList_Create(data);
      list->next->next = rover;
      list->next->prev = list;
      rover->prev = list->next;
   }
   else
   {
      list->next = SList_Create(data);
      list->next->prev = list;
   }
   
   return list->next;
}

slist_t *SList_InsertPrev(slist_t *list, listdata_t data)
{
   if(list->prev)
   {
      slist_t *rover = list->prev;
      list->prev = SList_Create(data);
      list->prev->next = list;
      list->prev->prev = rover;
      rover->next = list->prev;
   }
   else
   {
      list->prev = SList_Create(data);
      list->prev->next = list;
   }
   
   return list->prev;
}

size_t SList_Destroy(slist_t *list, size_t maxlength)
{
   size_t i = 0;
   
   for(slist_t *rover = list; rover && (maxlength == 0 || i < maxlength); i++)
   {
      slist_t *next = rover->next;
      
      if(rover->next)
         rover->next->prev = rover->prev;
      
      if(rover->prev)
         rover->prev->next = rover->next;
      
      free(rover);
      rover = next;
   }
   
   return i;
}

size_t SList_GetLength(slist_t *list)
{
   size_t size = 0;
   for(slist_t *rover = list; rover; rover = rover->next, size++);
   return size;
}

//
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Dynamic lists.
//

dlist_t *DList_Create(size_t length)
{
   dlist_t *list = malloc(sizeof(dlist_t));
   memset(list, 0, sizeof(dlist_t));
   
   if(length > 0)
   {
      list->head = SList_Create((listdata_t){ null });
      list->tail = list->head;
      
      if(length > 1)
      {
         slist_t *rover = list->head;
         size_t i = 0;
         
         length -= 1;
         while(i < length)
         {
            SList_InsertNext(rover);
            
            if(++i < length)
               rover = rover->next;
         }
         
         list->tail = rover;
      }
   }
   
   return list;
}

void DList_InsertBack(dlist_t *list, listdata_t data)
{
   if(list->tail)
   {
      list->tail = SList_InsertNext(list->tail, data);
   }
   else
   {
      list->head = SList_Create(data);
      list->tail = list->head;
   }
}

void DList_InsertFront(dlist_t *list, listdata_t data)
{
   if(list->head)
   {
      list->head = SList_InsertPrev(list->head, data);
   }
   else
   {
      list->head = SList_Create(data);
      list->tail = list->head;
   }
}

void DList_DeleteFront(dlist_t *list)
{
   if(list->head)
   {
      if(list->tail == list->head)
      {
         DList_Destroy(list);
      }
      else
      {
         slist_t *rover = list->head->next;
         SList_Destroy(list->head, 1);
         list->head = rover;
      }
   }
}

void DList_DeleteBack(dlist_t *list)
{
   if(list->tail)
   {
      if(list->tail == list->head)
      {
         DList_Destroy(list);
      }
      else
      {
         slist_t *rover = list->tail->prev;
         SList_Destroy(list->tail, 1);
         list->tail = rover;
      }
   }
}

size_t DList_GetLength(dlist_t *list)
{
   if(!list->head)
   {
      return 0;
   }
   
   return SList_GetLength(list->head);
}

void DList_Destroy(dlist_t *list)
{
   SList_Destroy(list->head);
   list->head = null;
   list->tail = null;
}

void DList_Free(dlist_t *list)
{
   SList_Destroy(list->head);
   free(list);
}

//
// ---------------------------------------------------------------------------

