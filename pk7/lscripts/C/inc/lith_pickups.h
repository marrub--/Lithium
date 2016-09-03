#ifndef LITH_PICKUPS_H
#define LITH_PICKUPS_H

struct pickupfmt_s
{
   int flag;
   __str fmt;
};

extern __str *const pickupnames[];
extern struct pickupfmt_s const pickupfmt[];
extern __str uncertainty[];

extern int const pickupnamesmax;
extern int const pickupfmtmax;
extern int const uncertaintymax;

#endif

