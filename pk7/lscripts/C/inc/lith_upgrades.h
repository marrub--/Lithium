#ifndef LITH_UPGRADES_H
#define LITH_UPGRADES_H

enum
{
   #define U(en, name) UPGR_##en,
   #include "lith_upgradenames.h"
};

typedef struct upgrades_s
{
   void *no_data;
} upgrades_t;

extern __str upgrade_enums[];
extern __str upgrade_names[];

extern int const upgrade_enums_max;
extern int const upgrade_names_max;

#endif

