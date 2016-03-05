#ifndef LITH_PICKUPS_H
#define LITH_PICKUPS_H

#define pickups(name, ...) static __str const pickupnames_##name[] = __VA_ARGS__;
#include "lith_pickupnames.h"
static __str const *const pickupnames[] = {
   #define pickups(name, ...) [name] = pickupnames_##name,
   #include "lith_pickupnames.h"
};

static struct { int flag; __str fmt; } const pickupfmt[] = {
   0, "\CjYou got the %S!",
   0, "\CjOh yes, the %S.",
   0, "\CjThe %S has been acquired.",
   0, "\CjSnatched up a %S!",
   0, "\CjOh baby, it's time for %S!",
   0, "\CjThere was a %S here, but you stole it. It is now in your inventory. You Monster.",
   0, "\CjAcquired a %S.",
   0, "\CjThe %S is safe and sound with you.",
   0, "\CjI bet that %S totally wanted you to pick it up.",
   0, "\CjIt's not like the %S wanted you to pick it up or anything.",
   0, "\CjIt's a %S... I think.",
   1, "\CjYou thought you picked up a %S ... %S",
   0, "\Cj1 in 3 of every %Ss do not approve of being picked up.",
   0, "\CjStudies show that normal people do not steal %Ss.",
   0, "\CjOH NO, NOT %S"
};

static __str const uncertainty[] = {
   "Yes.",
   "Maybe.",
   "Hmm, yes. Indeed.",
   "Aaaaand you did."
};

static int const pickupnamesmax = sizeof(pickupnames) / sizeof(*pickupnames);
static int const pickupfmtmax = sizeof(pickupfmt) / sizeof(*pickupfmt);
static int const uncertaintymax = sizeof(uncertainty) / sizeof(*uncertainty);

#endif

