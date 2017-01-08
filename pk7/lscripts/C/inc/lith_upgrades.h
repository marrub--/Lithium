#ifndef LITH_UPGRADES_H
#define LITH_UPGRADES_H

enum
{
   UC_MIN,
   UC_Body = UC_MIN,
   UC_Weap,
   UC_Extr,
   UC_Down,
   UC_MAX
};

enum
{
   UPGR_JetBooster,
   UPGR_ReflexWetw,
   UPGR_CyberLegs,
   UPGR_ReactArmour,
   
   UPGR_GaussShotty,
   UPGR_RifleModes,
   UPGR_ChargeRPG,
   UPGR_PlasLaser,
   UPGR_OmegaRail,
   
   UPGR_TorgueMode,
// UPGR_RetroWeps,
   UPGR_7777777,
   UPGR_lolsords,
   
   UPGR_Implying,
   UPGR_UNCEUNCE,
   
   UPGR_MAX
};

typedef void (*upgr_cb_t)(struct player_s *, struct upgrade_s *);
[[__call("ScriptS")]] typedef void (*upgr_update_cb_t)(struct player_s *, struct upgrade_s *);

typedef struct upgradeinfo_s
{
   __str name;
   score_t cost; // If this is exactly 0, you already own it.
   bool auto_activate; // If this is true, as soon as you own it you'll activate it.
   __str bipunlock;
   int category;
   fixed scoreadd;
   
   upgr_cb_t Activate, Deactivate;
   upgr_update_cb_t Update;
} upgradeinfo_t;

typedef struct upgrade_s
{
   bool active : 1, owned : 1, wasactive : 1;
   upgradeinfo_t const *info;
   int user_int[8];
   __str user_str[8];
} upgrade_t;

typedef upgrade_t upgrades_t[UPGR_MAX];

void Upgr_ToggleActive(struct player_s *p, upgrade_t *upgr);
void Upgr_SetOwned(struct player_s *p, upgrade_t *upgr);
bool Upgr_CanBuy(struct player_s *p, upgrade_t *upgr);
void Upgr_Buy(struct player_s *p, upgrade_t *upgr);

void Lith_PlayerInitUpgrades(struct player_s *p);
void Lith_PlayerUpdateUpgrades(struct player_s *p);
void Lith_PlayerDeinitUpgrades(struct player_s *p);
void Lith_PlayerReinitUpgrades(struct player_s *p);

#endif

