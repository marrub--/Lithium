#ifndef LITH_PICKUPS_H
#define LITH_PICKUPS_H

#define pickups(name, ...) static __str const pickupnames_##name[] = __VA_ARGS__;
#include "lith_pickupnames.h"
static __str const *const pickupnames[] = {
   #define pickups(name, ...) [name] = pickupnames_##name,
   #include "lith_pickupnames.h"
};

static __str const pickupfmt[] = {
   "\CjYou got the %S!",
   "\CjOh yes, a %S.",
   "\CjThe %S has been acquired.",
   "\CjSnatched up a %S!",
   "\CjOh baby, it's time for %S!",
   "\CjThere was a %S here, but you stole it.\nIt is now in your inventory.\nYou Monster.",
   "\CjOH NO, NOT %S"
};

static size_t const pickupfmtmax = sizeof(pickupfmt) / sizeof(*pickupfmt);

#endif

