/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Upgrade data and handling functions.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef u_data_h
#define u_data_h

#include "p_shopdef.h"

#define UPGR_STATIC_MAX 80
#define UPGR_EXTRA_NUM (UPGR_STATIC_MAX - UPGR_BASE_MAX)

#define CastUData(u) ((struct upgr_data *)(u)->dataptr)
#define UData_Adrenaline(u)  (CastUData(u)->u01)
#define UData_JetBooster(u)  (CastUData(u)->u03)
#define UData_lolsords(u)    (CastUData(u)->u04)
#define UData_ReactArmor(u)  (CastUData(u)->u05)
#define UData_ReflexWetw(u)  (CastUData(u)->u06)
#define UData_VitalScan(u)   (CastUData(u)->u07)
#define UData_Zoom(u)        (CastUData(u)->u08)
#define UData_HomingRPG(u)   (CastUData(u)->u09)
#define UData_Magic(u)       (CastUData(u)->u10)
#define UData_StealthSys(u)  (CastUData(u)->u11)
#define UData_HeadsUpDisp(u) (CastUData(u)->u13)

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

struct upgr_data_Adrenaline {
   i32  charge;
   bool readied;
};

struct upgr_data_JetBooster {
   i32  charge;
   bool discharged;
};

struct upgr_data_lolsords {
   str origweapon;
};

struct upgr_data_ReactArmor {
   i32 activearmor;
};

struct upgr_data_ReflexWetw {
   u32  charge;
   bool leaped;
};

struct upgr_data_VitalScan {
   i32 target;
   i32 oldtarget;
   str tagstr;
   u32 health;
   u32 oldhealth;
   u32 maxhealth;
   u32 hdelta;
   u32 hdtime;
   k64 cangle;
   k64 oangle;
   k64 splitfrac;
   u32 split;
   u32 rank;
   bool freak;
};

struct upgr_data_Zoom {
   i32 zoom;
   k64 vzoom;
};

struct upgr_data_HomingRPG {
   i32 id;
};

struct upgr_data_Magic {
   k32 manaperc;
   struct gui_state gst;
   bool ui;
};

struct upgr_data_StealthSys {
   k32 mulvel;
};

struct upgr_data_HeadsUpDisp {
   i32 cr;
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

struct upgradeinfo {
   anonymous struct shopdef shopdef;

   i32 pclass;
   i32 category;
   i32 perf;
   i32 group;
   i32 requires;
   k32 scoreadd;

   i32 key, id;

   stkcall void (*Activate)(struct player *, struct upgrade *);
   stkcall void (*Deactivate)(struct player *, struct upgrade *);
   script  void (*Update)(struct player *, struct upgrade *);
   stkcall void (*Enter)(struct player *, struct upgrade *);
   stkcall void (*Render)(struct player *, struct upgrade *);
   stkcall void (*Init)(struct player *, struct upgrade *);
};

bool P_Upg_CanActivate(struct player *p, struct upgrade *upgr);
bool P_Upg_Toggle(struct player *p, struct upgrade *upgr);
void P_Upg_SetOwned(struct player *p, struct upgrade *upgr);

struct upgrade {
   void *dataptr;

   struct upgrade *next, **prev;

   struct upgradeinfo const *info;

   bool active;
   bool owned;
   bool wasactive; /* for reinitializing on map load */
};

#endif
