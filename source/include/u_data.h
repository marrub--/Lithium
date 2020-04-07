/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
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

enum {
   UC_Body,
   UC_Weap,
   UC_Extr,
   UC_Down,
   UC_MAX
};

enum {
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

enum {
   UR_AI,
   UR_WMD,
   UR_WRD,
   UR_RDI,
   UR_RA,
};

struct upgr_data_7777777 {
   k32  fvel, fyaw;
   bool in_air;
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
   k32 exp;
};

struct upgr_data_Zoom {
   i32 zoom;
   k64 vzoom;
};

struct upgr_data_Flashlight {
   bool on, was_on;
   i32 battery;
   k64 intensity, target, speed;
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

struct upgr_data_HeadsUpDisM {
   i32 cr;
};

struct upgr_data_HeadsUpDisI {
   struct interp_data_lli score;
   struct interp_data_lli health;
   struct interp_data_lli overdrive;
};

struct upgr_data {
   struct upgr_data_7777777     seven7;
   struct upgr_data_Adrenaline  adrenaline;
   struct upgr_data_Flashlight  flashlight;
   struct upgr_data_HeadsUpDisI headsupdisi;
   struct upgr_data_HeadsUpDisM headsupdism;
   struct upgr_data_HomingRPG   homingrpg;
   struct upgr_data_JetBooster  jetbooster;
   struct upgr_data_Magic       magic;
   struct upgr_data_ReactArmor  reactarmor;
   struct upgr_data_ReflexWetw  reflexwetw;
   struct upgr_data_StealthSys  stealthsys;
   struct upgr_data_VitalScan   vitalscan;
   struct upgr_data_Zoom        zoom;
   struct upgr_data_lolsords    lolsords;
};

struct upgradeinfo {
   anonymous struct shopdef shopdef;

   i32 pclass;
   i32 category;
   i32 perf;
   i32 group;
   i32 requires;
   k32 scoreadd;

   i32 key;

   stkcall void (*Activate)(struct player *, struct upgrade *);
   stkcall void (*Deactivate)(struct player *, struct upgrade *);
   script  void (*Update)(struct player *, struct upgrade *);
   stkcall void (*Enter)(struct player *, struct upgrade *);
   stkcall void (*Render)(struct player *, struct upgrade *);
   stkcall void (*Init)(struct player *, struct upgrade *);
};

struct upgrade {
   struct upgrade *next, **prev;

   struct upgradeinfo const *info;

   u32 agroups;

   /* TODO: make flags instead of bools */
   bool available;
   bool active;
   bool owned;
   bool wasactive; /* for reinitializing on map load */
};

bool P_Upg_CanActivate(struct player *p, struct upgrade *upgr);
bool P_Upg_Toggle(struct player *p, struct upgrade *upgr);
void P_Upg_SetOwned(struct player *p, struct upgrade *upgr);

i32 Upgr_StrToEnum(cstr s);
cstr Upgr_EnumToStr(i32 n);

#endif
