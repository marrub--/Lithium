#ifndef LITH_TYCHO_H
#define LITH_TYCHO_H

#include <stdfix.h>
#include <stdbool.h>

typedef long long int score_t;

typedef struct shopdef_s
{
   // Public Data
   __str name;
   __str bipunlock;
   score_t cost;
   
   // Private Data
   void (*shopBuy)   (struct player_s *p, struct shopdef_s const *def, void *obj);
   bool (*shopCanBuy)(struct player_s *p, struct shopdef_s const *def, void *obj);
   void (*shopGive)  (int tid,            struct shopdef_s const *def, void *obj);
} shopdef_t;

#define UC_Body 0
#define UC_Weap 1
#define UC_Extr 2
#define UC_Down 3
#define UC_MAX  4

#define UPGR_STATIC_MAX 80
#define UPGR_EXTRA_NUM (UPGR_STATIC_MAX - UPGR_BASE_MAX)

#define UserData_Adrenaline data.u01
#define UserData_Implying   data.u02
#define UserData_JetBooster data.u03
#define UserData_lolsords   data.u04
#define UserData_ReactArmor data.u05
#define UserData_ReflexWetw data.u06
#define UserData_VitalScan  data.u07
#define UserData_Zoom       data.u08
#define UserData_HomingRPG  data.u09
#define UserData_Custom     data.u10

                      typedef void (*upgr_cb_t)       (struct player_s *, struct upgrade_s *);
[[__call("ScriptS")]] typedef void (*upgr_update_cb_t)(struct player_s *, struct upgrade_s *);

enum
{
   UG_None,
   UG_Pistol,
   UG_Shotgun,
   UG_Launcher,
   UG_Plasma,
   UG_BFG,
   UG_HUD,
   UG_BASE_MAX
};

enum
{
   UR_AI  = 1 << 0,
   UR_WMD = 1 << 1,
   UR_WRD = 1 << 2,
   UR_RDI = 1 << 3,
   UR_RA  = 1 << 4,
};

union upgradedata_u
{
   // Private Data
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
   
   struct {int id;} u09;
   
   // Public Data
   struct {void *userdata;} u10;
};

typedef struct upgradeinfo_s
{
   // Public Data
   [[__anonymous]] shopdef_t shopdef;
   int   pclass;
   int   category;
   int   perf;
   fixed scoreadd;
   int   group;
   unsigned requires;
   
   // Readonly Data
   int id, key;
   
   upgr_cb_t        Activate;
   upgr_cb_t        Deactivate;
   upgr_update_cb_t Update;
   upgr_cb_t        Render;
   upgr_cb_t        Enter;
} upgradeinfo_t;

bool Lith_UpgrCanActivate(struct player_s *p, struct upgrade_s *upgr);
bool Lith_UpgrToggle(struct player_s *p, struct upgrade_s *upgr);
void Lith_UpgrSetOwned(struct player_s *p, struct upgrade_s *upgr);

typedef struct upgrade_s
{
   __prop canUse   {call: Lith_UpgrCanActivate(__arg, this)}
   __prop toggle   {call: Lith_UpgrToggle(__arg, this)}
   __prop setOwned {call: Lith_UpgrSetOwned(__arg, this)}
   
   // Public Data
   union upgradedata_u data;
   
   // Readonly Data
   upgradeinfo_t const *info;
   
   bool active : 1;
   bool owned  : 1;
   
   // Private Data
   bool wasactive : 1; // for reinitializing on map load
} upgrade_t;

void Lith_LoadUpgrInfoBalance(upgradeinfo_t *uinfo, int max, char const *fname);
[[__call("ScriptS"), __extern("ACS")]]
void Lith_RegisterBasicUpgrade(int key, __str name, int cost, int category);

#endif

