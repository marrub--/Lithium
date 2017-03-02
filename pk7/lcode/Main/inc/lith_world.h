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

typedef struct payoutinfo_s
{
   int killnum, killmax;
   int itemnum, itemmax;
   
   long fixed killpct;
   long fixed itempct;
   
   int killscr;
   int itemscr;
   
   int total;
   int tax;
} payoutinfo_t;

__addrdef extern __mod_arr Lith_MapVariable;
__addrdef extern __hub_arr Lith_WorldVariable;

extern bool Lith_MapVariable mapinit;
extern int  Lith_MapVariable mapid;

extern int secretsfound;
extern payoutinfo_t payout;
extern int mapseed;

[[__optional_args(1)]] int Lith_UniqueID(int tid);

#endif

