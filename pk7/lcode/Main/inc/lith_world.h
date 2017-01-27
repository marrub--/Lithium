#ifndef LITH_WORLD_H
#define LITH_WORLD_H

#include <stdbool.h>

enum
{
   skill_tourist,
   skill_easy,
   skill_normal,
   skill_hard,
   skill_extrahard,
   skill_nightmare
};

__addrdef extern __mod_arr Lith_MapVariable;

extern bool Lith_MapVariable mapinit;

#endif

