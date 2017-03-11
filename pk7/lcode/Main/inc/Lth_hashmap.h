//-----------------------------------------------------------------------------
//
// Copyright © 2016-2017 Project Golan, Team Doominati
//
// Code by Team Doominati has been relicensed with consent.
//
// See "LICENSE.lithos3" for more information.
//
//-----------------------------------------------------------------------------
//
// Fixed-size (runtime) hash map.
//
//-----------------------------------------------------------------------------

#ifndef lithos3__Lth_hashmap_h
#define lithos3__Lth_hashmap_h

#include "Lth_stdlib.h"

#include <wchar.h>
#include <stdlib.h>

#define Lth_GenFind(hashexpr) \
   Lth_HashMapElem *elem = Lth_HashFindItr(map, (hashexpr)); \
   if(elem == NULL) return NULL; \
   return elem->value

#define Lth_HashMapFind(map, expr) \
   _Generic((expr)+0, \
      char __str_ars const *: Lth_HashMapFind_str,   \
      char                 *: Lth_HashMapFind_char,  \
      char           const *: Lth_HashMapFind_char,  \
      wchar_t              *: Lth_HashMapFind_wchar, \
      wchar_t        const *: Lth_HashMapFind_wchar, \
      size_t:                 Lth_HashMapFind_hash,  \
      wchar_t:                Lth_HashMapFind_hash,  \
      int:                    Lth_HashMapFind_hash)((map), (expr))


// Type Definitions ----------------------------------------------------------|

//
// Lth_HashMapElem
//
typedef struct Lth_HashMapElem
{
   size_t keyhash;
   void  *value;
   struct Lth_HashMapElem *next;
} Lth_HashMapElem;

//
// Lth_HashMap
//
// internal data
//    table
//
// read-only
//    elem
//
typedef struct Lth_HashMap
{
   Lth_HashMapElem           **table;
   Lth_Vector(Lth_HashMapElem) elem;
} Lth_HashMap;


// Static Functions ----------------------------------------------------------|

//
// Lth_HashMapDestroy
//
static inline void Lth_HashMapDestroy(Lth_HashMap *map)
{
   if(map == NULL) return;

   free(map->elem.data);

   map->table     = NULL;
   map->elem.data = NULL;
   map->elem.size = 0;
}

//
// Lth_HashMapAlloc
//
static inline void Lth_HashMapAlloc(Lth_HashMap *map, size_t count)
{
   if(map->elem.data)
      Lth_HashMapDestroy(map);

   size_t size =
      sizeof(Lth_HashMapElem) * count + sizeof(Lth_HashMapElem *) * count;

   map->elem.size = count;
   map->elem.data = calloc(1, size);
}

//
// Lth_HashMapBuild
//
static inline void Lth_HashMapBuild(Lth_HashMap *map)
{
   map->table = (Lth_HashMapElem **)(map->elem.data + map->elem.size);

   for(Lth_HashMapElem **elem = map->table + map->elem.size;
      elem != map->table;)
      *--elem = NULL;

   for(size_t i = 0; i < map->elem.size; i++)
   {
      Lth_HashMapElem *elem = &map->elem.data[i];
      size_t hash = elem->keyhash % map->elem.size;
      elem->next = map->table[hash];
      map->table[hash] = elem;
   }
}

//
// Lth_HashMapFindItr
//
static inline Lth_HashMapElem *Lth_HashFindItr(Lth_HashMap *map, size_t hash)
{
   if(map->table == NULL) return NULL;

   for(Lth_HashMapElem *elem = map->table[hash % map->elem.size]; elem;
      elem = elem->next)
   {
      if(elem->keyhash == hash)
         return elem;
   }

   return NULL;
}

//
// Lth_HashMapFind_char
//
static inline void *Lth_HashMapFind_char(Lth_HashMap *map, char const *key)
{
   Lth_GenFind(Lth_Hash_char(key));
}

//
// Lth_HashMapFind_str
//
static inline void *Lth_HashMapFind_str(Lth_HashMap *map,
   char __str_ars const *key)
{
   Lth_GenFind(Lth_Hash_str(key));
}

//
// Lth_HashMapFind_wchar
//
static inline void *Lth_HashMapFind_wchar(Lth_HashMap *map, wchar_t const *key)
{
   Lth_GenFind(Lth_Hash_wchar(key));
}

//
// Lth_HashMapFind_hash
//
static inline void *Lth_HashMapFind_hash(Lth_HashMap *map, size_t key)
{
   Lth_GenFind(key);
}

#undef Lth_GenFind

#endif//lithos3__Lth_hashmap_h
