#ifndef LITH_MONSTER_H
#define LITH_MONSTER_H

#include "lith_common.h"
#include "lith_list.h"

#define DMON_MAX 0x7FFF
#define DMONHASH_MAX 0x7FFF

typedef struct dmon_s
{
   bool active;
   int id;
   list_t link;
   
   int level;
} dmon_t;

dmon_t *Dmon(int id);

extern list_t dmonhash[DMONHASH_MAX];

#endif//LITH_MONSTER_H

