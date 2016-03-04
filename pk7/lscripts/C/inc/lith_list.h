#ifndef LITH_LIST_H
#define LITH_LIST_H

#include <stddef.h>

typedef union listdata_u
{
   void *vp;
   __str str;
} listdata_t;

typedef struct slist_s
{
   listdata_t data;
   
   struct slist_s *next;
   struct slist_s *prev;
} slist_t;

typedef struct dlist_s
{
   slist_t *head;
   slist_t *tail;
} dlist_t;

[[__optional_args(1)]]
slist_t *SList_Create(listdata_t data);
[[__optional_args(1)]]
slist_t *SList_InsertNext(slist_t *list, listdata_t data);
[[__optional_args(1)]]
slist_t *SList_InsertPrev(slist_t *list, listdata_t data);
[[__optional_args(1)]]
size_t SList_Destroy(slist_t *list, size_t maxlength);
size_t SList_GetLength(slist_t *list);

[[__optional_args(1)]]
dlist_t *DList_Create(size_t length);
void DList_InsertBack(dlist_t *list, listdata_t data);
void DList_InsertFront(dlist_t *list, listdata_t data);
void DList_DeleteFront(dlist_t *list);
void DList_DeleteBack(dlist_t *list);
size_t DList_GetLength(dlist_t *list);
void DList_Destroy(dlist_t *list);
void DList_Free(dlist_t *list);

#endif

