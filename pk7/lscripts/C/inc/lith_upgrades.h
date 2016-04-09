#ifndef LITH_UPGRADES_H
#define LITH_UPGRADES_H

typedef void (*upgr_update_t)(struct player_s *, struct upgrade_s *);

typedef struct upgradeinfo_s
{
   score_t cost; // If this is exactly 0, you already own it.
   bool auto_activate; // If this is true, as soon as you own it you'll activate it.
   
   upgr_update_t Activate;
   upgr_update_t Deactivate;
   upgr_update_t Update;
} upgradeinfo_t;

typedef struct upgrade_s
{
   bool active, owned;
   upgradeinfo_t const *info;
   __str name;
   __str description;
   int user_int[8];
   __str user_str[8];
} upgrade_t;

enum
{
   #define U(en, name) UPGR_##en,
   #include "lith_upgradenames.h"
   UPGR_MAX
};

extern __str upgrade_names[];

void Upgr_ToggleActive(struct player_s *p, upgrade_t *upgr);
void Upgr_SetOwned(struct player_s *p, upgrade_t *upgr);
bool Upgr_CanBuy(struct player_s *p, upgrade_t *upgr);
void Upgr_Buy(struct player_s *p, upgrade_t *upgr);

void Lith_PlayerInitUpgrades(struct player_s *p);
void Lith_PlayerUpdateUpgrades(struct player_s *p);

#endif

