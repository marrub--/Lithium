// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#ifndef LITH_UPGRADEDATA_H
#define LITH_UPGRADEDATA_H

#include "lith_shopdef.h"

#define UPGR_STATIC_MAX 80
#define UPGR_EXTRA_NUM (UPGR_STATIC_MAX - UPGR_BASE_MAX)

#define Lith_CastUData(u) ((struct upgr_data *)(u)->dataptr)
#define UData_Adrenaline(u)  (Lith_CastUData(u)->u01)
#define UData_JetBooster(u)  (Lith_CastUData(u)->u03)
#define UData_lolsords(u)    (Lith_CastUData(u)->u04)
#define UData_ReactArmor(u)  (Lith_CastUData(u)->u05)
#define UData_ReflexWetw(u)  (Lith_CastUData(u)->u06)
#define UData_VitalScan(u)   (Lith_CastUData(u)->u07)
#define UData_Zoom(u)        (Lith_CastUData(u)->u08)
#define UData_HomingRPG(u)   (Lith_CastUData(u)->u09)
#define UData_Magic(u)       (Lith_CastUData(u)->u10)
#define UData_StealthSys(u)  (Lith_CastUData(u)->u11)
#define UData_HeadsUpDisp(u) (Lith_CastUData(u)->u13)

enum
{
   UC_Body,
   UC_Weap,
   UC_Extr,
   UC_Down,
   UC_MAX
};

enum
{
   UG_None,
   UG_Pistol,
   UG_Shotgun,
   UG_SSG,
   UG_Rifle,
   UG_Launcher,
   UG_Plasma,
   UG_BFG,
   UG_HUD,
};

enum
{
   UR_AI  = 1 << 0,
   UR_WMD = 1 << 1,
   UR_WRD = 1 << 2,
   UR_RDI = 1 << 3,
   UR_RA  = 1 << 4,
};

typedef bool (*upgr_reinit_cb_t)(struct upgradeinfo *ui);

                      typedef void (*upgr_fn_cb_t)(struct player *, struct upgrade *);
script typedef void (*upgr_sc_cb_t)(struct player *, struct upgrade *);

struct upgr_data_Adrenaline {
   int  charge;
   bool readied;
};

struct upgr_data_JetBooster {
   int charge;
   bool discharged : 1;
};

struct upgr_data_lolsords {
   __str origweapon;
};

struct upgr_data_ReactArmor {
   int activearmor;
};

struct upgr_data_ReflexWetw {
   u32  charge : 6;
   bool leaped : 1;
};

struct upgr_data_VitalScan {
   int   target;
   int   oldtarget;
   __str tagstr;
   int   health;
   int   oldhealth;
   int   maxhealth;
   fixed cangle;
   fixed oangle;
   fixed splitfrac;
   u32  split : 3;
   u32  rank  : 4;
   bool freak : 1;
};

struct upgr_data_Zoom {
   int     zoom;
   fixed64 vzoom;
};

struct upgr_data_HomingRPG {
   int id;
};

struct upgr_data_Magic {
   fixed manaperc;
   gui_state_t gst;
   bool ui : 1;
};

struct upgr_data_StealthSys {
   fixed mulvel;
};

struct upgr_data_HeadsUpDisp {
   int cr;
};

struct upgr_data {
   struct upgr_data_Adrenaline  u01;
   struct upgr_data_JetBooster  u03;
   struct upgr_data_lolsords    u04;
   struct upgr_data_ReactArmor  u05;
   struct upgr_data_ReflexWetw  u06;
   struct upgr_data_VitalScan   u07;
   struct upgr_data_Zoom        u08;
   struct upgr_data_HomingRPG   u09;
   struct upgr_data_Magic       u10;
   struct upgr_data_StealthSys  u11;
   struct upgr_data_HeadsUpDisp u13;
};

typedef struct upgradeinfo {
   anonymous shopdef_t shopdef;

   int   pclass;
   int   category;
   int   perf;
   fixed scoreadd;
   int   group;
   u32   requires;

   int id, key;

   upgr_fn_cb_t Activate;
   upgr_fn_cb_t Deactivate;
   upgr_sc_cb_t Update;
   upgr_fn_cb_t Enter;
   upgr_fn_cb_t Render;
   upgr_fn_cb_t Init;
} upgradeinfo_t;

bool Lith_UpgrCanActivate(struct player *p, struct upgrade *upgr);
bool Lith_UpgrToggle(struct player *p, struct upgrade *upgr);
void Lith_UpgrSetOwned(struct player *p, struct upgrade *upgr);

typedef struct upgrade {
   __prop canUse   {call: Lith_UpgrCanActivate(__arg, this)}
   __prop toggle   {call: Lith_UpgrToggle(__arg, this)}
   __prop setOwned {call: Lith_UpgrSetOwned(__arg, this)}

   void *dataptr;

   struct upgrade *next, **prev;

   upgradeinfo_t const *info;

   bool active    : 1;
   bool owned     : 1;
   bool wasactive : 1; // for reinitializing on map load
} upgrade_t;

void Lith_LoadUpgrInfoBalance(upgradeinfo_t *uinfo, int max, char const *fname);

#endif

