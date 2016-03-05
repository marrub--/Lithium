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
   "\CjOh yes, the %S.",
   "\CjThe %S has been acquired.",
   "\CjSnatched up a %S!",
   "\CjOh baby, it's time for %S!",
   "\CjThere was a %S here, but you stole it. It is now in your inventory. You Monster.",
   "\CjAcquired a %S.",
   "\CjThe %S is safe and sound with you.",
   "\CjI bet that %S totally wanted you to pick it up.",
   "\CjIt's not like the %S wanted you to pick it up or anything.",
   "\CjIt's a %S... I think.",
   "\CjYou thought you picked up a %S ... Aaaand it turns out you did.",
   "\Cj1 in 3 of every %Ss do not approve of being picked up.",
   "\CjStudies show that normal people do not steal %Ss.",
   "\CjOH NO, NOT %S"
};

static size_t const pickupfmtmax = sizeof(pickupfmt) / sizeof(*pickupfmt);

#endif

