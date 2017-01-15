#ifndef LITH_UPGRADES_H
#define LITH_UPGRADES_H

#include "lith_common.h"

//----------------------------------------------------------------------------
// Type Definitions
//

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
   UPGR_HeadsUpDisp,
   UPGR_JetBooster,
   UPGR_ReflexWetw,
   UPGR_CyberLegs,
   UPGR_ReactArmour,
   UPGR_DefenseNuke,
   UPGR_Adrenaline,
   
   UPGR_GaussShotty,
   UPGR_RifleModes,
   UPGR_ChargeRPG,
   UPGR_PlasLaser,
   UPGR_Punct,
   
   UPGR_TorgueMode,
// UPGR_RetroWeps,
   UPGR_7777777,
   UPGR_lolsords,
   
   UPGR_Implying,
   UPGR_UNCEUNCE,
   UPGR_InstaDeath,
   
   UPGR_MAX
};

enum
{
   u_charge     = 0,
   u_hudid      = 0,
   u_origweapon = 0,
   u_leaped     = 1,
   u_readied    = 1,
};

//
// upgr_cb_t
//
typedef void (*upgr_cb_t)(struct player_s *, struct upgrade_s *);

//
// upgr_update_cb_t
//
[[__call("ScriptS")]] typedef void (*upgr_update_cb_t)(struct player_s *, struct upgrade_s *);

//
// upgradeinfo_t
//
typedef struct upgradeinfo_s
{
   __str name;
   score_t cost; // If this is exactly 0, you already own it.
   bool auto_activate; // If this is true, as soon as you own it you'll activate it.
   __str bipunlock;
   int category;
   fixed scoreadd;
   int wepclass;
   
   upgr_cb_t        Activate;
   upgr_cb_t        Deactivate;
   upgr_update_cb_t Update;
   upgr_cb_t        Render;
   upgr_cb_t        Enter;
} upgradeinfo_t;

//
// upgrade_t
//
typedef struct upgrade_s
{
   bool active : 1, owned : 1, wasactive : 1;
   upgradeinfo_t const *info;
   int user_int[8];
   __str user_str[8];
} upgrade_t;

//
// upgrades_t
//
typedef upgrade_t upgrades_t[UPGR_MAX];


//----------------------------------------------------------------------------
// Extern Functions
//

#define A(n)                       void Upgr_##n##_Activate(struct player_s *p, upgrade_t *upgr);
#define D(n)                       void Upgr_##n##_Deactivate(struct player_s *p, upgrade_t *upgr);
#define U(n) [[__call("ScriptS")]] void Upgr_##n##_Update(struct player_s *p, upgrade_t *upgr);
#define E(n)                       void Upgr_##n##_Enter(struct player_s *p, upgrade_t *upgr);
#define R(n)                       void Upgr_##n##_Render(struct player_s *p, upgrade_t *upgr);

// A(-----------) D(-----------) U(-----------) E(-----------) R(-----------)
                                                               R(HeadsUpDisp)
   A(JetBooster)                 U(JetBooster)                 R(JetBooster)
   A(ReflexWetw)  D(ReflexWetw)  U(ReflexWetw)                 R(ReflexWetw)
   A(CyberLegs)   D(CyberLegs)   U(CyberLegs)
                  D(ReactArmour)
                                                E(DefenseNuke)
                                 U(Adrenaline)
//---------------------------------------------------------------------------
                  D(RifleModes)  U(RifleModes)
                  D(PunctCannon)
//---------------------------------------------------------------------------
   A(7777777)     D(7777777)     U(7777777)
   A(lolsords)    D(lolsords)    U(lolsords)
//---------------------------------------------------------------------------
                                 U(Implying)
   A(UNCEUNCE)    D(UNCEUNCE)    U(UNCEUNCE)
                                 U(InstaDeath)
//---------------------------------------------------------------------------

#undef A
#undef D
#undef U
#undef E
#undef R

void Upgr_ToggleActive(struct player_s *p, upgrade_t *upgr);
void Upgr_SetOwned(struct player_s *p, upgrade_t *upgr);
bool Upgr_CanBuy(struct player_s *p, upgrade_t *upgr);
void Upgr_Buy(struct player_s *p, upgrade_t *upgr);

void Lith_PlayerInitUpgrades(struct player_s *p);
void Lith_PlayerDeinitUpgrades(struct player_s *p);
void Lith_PlayerReinitUpgrades(struct player_s *p);
void Lith_PlayerLoseUpgrades(struct player_s *p);

void Lith_PlayerUpdateUpgrades(struct player_s *p);
void Lith_PlayerRenderUpgrades(struct player_s *p);
void Lith_PlayerEnterUpgrades(struct player_s *p);

#endif

