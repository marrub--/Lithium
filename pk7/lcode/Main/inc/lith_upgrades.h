#ifndef LITH_UPGRADES_H
#define LITH_UPGRADES_H

#include "lith_common.h"
#include "lith_upgradenames.h"

#define UserData_Adrenaline data.u01
#define UserData_Implying   data.u02
#define UserData_JetBooster data.u03
#define UserData_lolsords   data.u04
#define UserData_ReactArmor data.u05
#define UserData_ReflexWetw data.u06
#define UserData_VitalScan  data.u07


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

union upgradedata_u
{
   struct
   {
      int charge;
      bool readied;
   } u01;
   
   struct {int hudid;}        u02;
   struct {int charge;}       u03;
   struct {__str origweapon;} u04;
   struct {int activearmor;}  u05;
   
   struct
   {
      int charge;
      bool leaped;
   } u06;
   
   struct
   {
      int target;
      int oldtarget;
      __str tagstr;
      int health;
      int oldhealth;
      int maxhealth;
      float angle;
      float old;
   } u07;
};

                      typedef void (*upgr_cb_t)       (struct player_s *, struct upgrade_s *);
[[__call("ScriptS")]] typedef void (*upgr_update_cb_t)(struct player_s *, struct upgrade_s *);

typedef struct upgradeinfo_s
{
   __str name;
   __str bipunlock;
   score_t cost;
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
   union upgradedata_u data;
} upgrade_t;

//
// upgrades_t
//
typedef upgrade_t upgrades_t[UPGR_MAX];


//----------------------------------------------------------------------------
// Extern Functions
//

// NB: These are included here and not in lith_upgrades.c so the
//     function signatures can be checked by the compiler.

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
                  D(ReactArmor)
                                                E(DefenseNuke)
                                 U(Adrenaline)
                                 U(VitalScan)                  R(VitalScan)
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

