#ifndef LITH_MONSTER_H
#define LITH_MONSTER_H

#include "lith_common.h"
#include "lith_list.h"
#include "lith_world.h"

#define DMON_MAX 0x7FFF

struct dmon
{
   bool active;
   int id;
   
   int level;
};

typedef struct dmon lwvar dmon_t;

[[__call("ScriptS"), __optional_args(2)]] dmon_t *DmonPtr(int tid, int ptr);
dmon_t *Dmon(int id);
dmon_t *AllocDmon(void);

#endif//LITH_MONSTER_H

