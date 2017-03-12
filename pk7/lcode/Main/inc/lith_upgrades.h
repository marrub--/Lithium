#ifndef LITH_UPGRADES_H
#define LITH_UPGRADES_H

#include "lith_common.h"
#include "lith_upgradenames.h"
#include "lith_shopdef.h"

#define UserData_Adrenaline data.u01
#define UserData_Implying   data.u02
#define UserData_JetBooster data.u03
#define UserData_lolsords   data.u04
#define UserData_ReactArmor data.u05
#define UserData_ReflexWetw data.u06
#define UserData_VitalScan  data.u07
#define UserData_Zoom       data.u08

#define NUMAUTOGROUPS 4

#define Lith_UpgrBuy(p, upgr) \
   Lith_ShopBuy(p, &(upgr)->info->shopdef, (upgr), "LITH_TXT_UPGRADE_TITLE_%S")


//----------------------------------------------------------------------------
// Types
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
   ug_none,
   ug_pistol,
   ug_shotgun,
   ug_rifle,
   ug_launcher,
   ug_plasma,
   ug_bfg,
   ug_max
};

//
// upgradedata_u
//
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
      bool freak;
   } u07;
   
   struct
   {
      int zoom;
      float vzoom;
   } u08;
};

                      typedef void (*upgr_cb_t)       (struct player_s *, struct upgrade_s *);
[[__call("ScriptS")]] typedef void (*upgr_update_cb_t)(struct player_s *, struct upgrade_s *);

//
// upgradeinfo_t
//
typedef struct upgradeinfo_s
{
   [[__anonymous]] shopdef_t shopdef;
   int category;
   fixed scoreadd;
   int group;
   
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
   bool active    : 1;
   bool owned     : 1;
   bool wasactive : 1; // for reinitializing on map load
   
   upgradeinfo_t const *info;
   union upgradedata_u  data;
   
   bool autogroups[NUMAUTOGROUPS];
} upgrade_t;

//
// upgrades_t
//
typedef upgrade_t upgrades_t[UPGR_MAX];


//----------------------------------------------------------------------------
// Extern Objects
//

extern __str Lith_AutoGroupNames[NUMAUTOGROUPS];


//----------------------------------------------------------------------------
// Extern Functions
//

// These are included here so the compiler may check the function signatures.
#define A(n)                       void Upgr_##n##_Activate  (struct player_s *p, upgrade_t *upgr);
#define D(n)                       void Upgr_##n##_Deactivate(struct player_s *p, upgrade_t *upgr);
#define U(n) [[__call("ScriptS")]] void Upgr_##n##_Update    (struct player_s *p, upgrade_t *upgr);
#define E(n)                       void Upgr_##n##_Enter     (struct player_s *p, upgrade_t *upgr);
#define R(n)                       void Upgr_##n##_Render    (struct player_s *p, upgrade_t *upgr);
#include "lith_upgradefuncs.h"


void Lith_PlayerInitUpgrades(struct player_s *p);
void Lith_PlayerDeinitUpgrades(struct player_s *p);
void Lith_PlayerReinitUpgrades(struct player_s *p);
void Lith_PlayerLoseUpgrades(struct player_s *p);

void Lith_PlayerUpdateUpgrades(struct player_s *p);
void Lith_PlayerRenderUpgrades(struct player_s *p);
void Lith_PlayerEnterUpgrades(struct player_s *p);

bool Lith_UpgrToggle(struct player_s *p, upgrade_t *upgr);

#endif

