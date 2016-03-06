#ifndef LITH_PICKUPS_H
#define LITH_PICKUPS_H

#define pickups(name, ...) static __str const pickupnames_##name[] = __VA_ARGS__;
#include "lith_pickupnames.h"
static __str const *const pickupnames[] = {
   #define pickups(name, ...) [name] = pickupnames_##name,
   #include "lith_pickupnames.h"
};

// Flags
// 1: Don't include the last thing in the uncertainty array in randomized certainty.
static struct { int flag; __str fmt; } const pickupfmt[] = {
   0, "You got the %S!",
   0, "Oh yes, the %S.",
   0, "The %S has been acquired.",
   0, "Snatched up a %S!",
   0, "Oh baby, it's time for %S!",
   0, "There was a %S here, but you stole it. It is now in your inventory. You Monster.",
   0, "Acquired a %S.",
   0, "The %S is safe and sound with you.",
   0, "I bet that %S totally wanted you to pick it up.",
   0, "It's not like the %S wanted you to pick it up or anything.",
   0, "It's a %S... I think.",
   0, "You thought you picked up a %S ... %S",
   0, "1 in 3 of every %Ss do not approve of being picked up.",
   0, "Studies show that normal people do not steal %Ss.",
   0, "OH NO, NOT %S",
   1, "What? Is that.. %S? ... %S",
   0, "Magnetically attached a %S to your body, crushing your every bone. (Just kidding.)",
   0, "DavidPH was here, nyu.",
   0, "%S-senpai still won't notice you.",
   0, "Grabbed a %S!",
   0, "Snatched up a %S!",
   0, "Embraced the %S carefully!",
   0, "There's a thing that looks like a %S here. You picked it up. Past-tense.",
   0, "Another day, another %S.",
   0, "Xaser didn't make this %S, so it sucks. You should drop it.",
};

static __str const uncertainty[] = {
   "Yes.",
   "Maybe.",
   "Hmm, yes. Indeed.",
   "Yeah.",
   "Aaaaand you did."
};

static int const pickupnamesmax = sizeof(pickupnames) / sizeof(*pickupnames);
static int const pickupfmtmax = sizeof(pickupfmt) / sizeof(*pickupfmt);
static int const uncertaintymax = sizeof(uncertainty) / sizeof(*uncertainty);

#endif

