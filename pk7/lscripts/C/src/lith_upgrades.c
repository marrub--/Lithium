#include "lith_upgrades.h"
#include "lith_common.h"

__str upgrade_enums[] = {
   #define U(en, name) [UPGR_##en] = #en,
   #include "lith_upgradenames.h"
   null
};

__str upgrade_names[] = {
   #define U(en, name) [UPGR_##en] = name,
   #include "lith_upgradenames.h"
   null
};

int const upgrade_enums_max = (sizeof(upgrade_enums) / sizeof(*upgrade_enums)) - 1;
int const upgrade_names_max = (sizeof(upgrade_names) / sizeof(*upgrade_names)) - 1;

