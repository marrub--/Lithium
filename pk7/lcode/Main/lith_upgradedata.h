#ifndef LITH_UPGRADEDATA_H
#define LITH_UPGRADEDATA_H

#include "lith_shopdef.h"

#define UPGR_STATIC_MAX 80
#define UPGR_EXTRA_NUM (UPGR_STATIC_MAX - UPGR_BASE_MAX)

#define CastUData(u) ((struct upgr_data *)(u)->dataptr)
#define UData_Adrenaline(u) (CastUData(u)->u01)
#define UData_Implying(u)   (CastUData(u)->u02)
#define UData_JetBooster(u) (CastUData(u)->u03)
#define UData_lolsords(u)   (CastUData(u)->u04)
#define UData_ReactArmor(u) (CastUData(u)->u05)
#define UData_ReflexWetw(u) (CastUData(u)->u06)
#define UData_VitalScan(u)  (CastUData(u)->u07)
#define UData_Zoom(u)       (CastUData(u)->u08)
#define UData_HomingRPG(u)  (CastUData(u)->u09)
#define UData_Magic(u)      (CastUData(u)->u10)

enum
{
   UC_Body,
   UC_Weap,
   UC_Extr,
   UC_Down,
   UC_MAX
};

                      typedef void (*upgr_fn_cb_t)(struct player *, struct upgrade_s *);
[[__call("ScriptS")]] typedef void (*upgr_sc_cb_t)(struct player *, struct upgrade_s *);

struct upgr_data_Adrenaline {
   int  charge;
   bool readied;
};

struct upgr_data_Implying {
   int hudid;
};

struct upgr_data_JetBooster {
   int charge;
};

struct upgr_data_lolsords {
   __str origweapon;
};

struct upgr_data_ReactArmor {
   int activearmor;
};

struct upgr_data_ReflexWetw {
   int  charge;
   bool leaped;
};

struct upgr_data_VitalScan {
   int   target;
   int   oldtarget;
   __str tagstr;
   int   health;
   int   oldhealth;
   int   maxhealth;
   float cangle;
   float oangle;
   fixed splitfrac;
   unsigned split : 3;
   unsigned rank  : 4;
   bool     freak : 1;
};

struct upgr_data_Zoom {
   int   zoom;
   float vzoom;
};

struct upgr_data_HomingRPG {
   int id;
};

struct upgr_data_Magic {
   fixed manaperc;
   gui_state_t gst;
   bool ui : 1;
};

struct upgr_data {
   struct upgr_data_Adrenaline u01;
   struct upgr_data_Implying   u02;
   struct upgr_data_JetBooster u03;
   struct upgr_data_lolsords   u04;
   struct upgr_data_ReactArmor u05;
   struct upgr_data_ReflexWetw u06;
   struct upgr_data_VitalScan  u07;
   struct upgr_data_Zoom       u08;
   struct upgr_data_HomingRPG  u09;
   struct upgr_data_Magic      u10;
};

typedef struct upgradeinfo_s {
   [[__anonymous]] shopdef_t shopdef;
   int   pclass;
   int   category;
   int   perf;
   fixed scoreadd;
   int   group;
   unsigned requires;
   
   int id, key;
   
   upgr_fn_cb_t Activate;
   upgr_fn_cb_t Deactivate;
   upgr_sc_cb_t Update;
   upgr_fn_cb_t Render;
   upgr_fn_cb_t Enter;
} upgradeinfo_t;

bool Lith_UpgrCanActivate(struct player *p, struct upgrade_s *upgr);
bool Lith_UpgrToggle(struct player *p, struct upgrade_s *upgr);
void Lith_UpgrSetOwned(struct player *p, struct upgrade_s *upgr);

typedef struct upgrade_s {
   __prop canUse   {call: Lith_UpgrCanActivate(__arg, this)}
   __prop toggle   {call: Lith_UpgrToggle(__arg, this)}
   __prop setOwned {call: Lith_UpgrSetOwned(__arg, this)}
   
   void *dataptr;
   
   upgradeinfo_t const *info;
   
   bool active    : 1;
   bool owned     : 1;
   bool wasactive : 1; // for reinitializing on map load
} upgrade_t;

void Lith_LoadUpgrInfoBalance(upgradeinfo_t *uinfo, int max, char const *fname);

#endif

