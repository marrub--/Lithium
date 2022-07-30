// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Upgrade data and handling functions.                                     │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#if defined(upgr_category_x)
upgr_category_x(body)
upgr_category_x(weap)
upgr_category_x(extr)
upgr_category_x(down)
#undef upgr_category_x
#elif !defined(u_data_h)
#define u_data_h

#include "p_shopdef.h"

enum {
   #define upgr_category_x(name) _uc_##name,
   #include "u_data.h"
   _uc_max,
};

enum {
   _ur_ai,
   _ur_wmd,
   _ur_wrd,
   _ur_rdi,
   _ur_ra,
   _ur_max,
};

enum {
   _ug_available,
   _ug_active,
   _ug_owned,
   _ug_wasactive, /* for reinitializing */
};

struct upgr_data_Seven7s {
   k32  fvel, fyaw;
   bool in_air;
};

struct upgr_data_lolsords {
   str origweapon;
};

struct upgr_data_ReactArmor {
   i32 activearmor;
};

struct upgr_data_ReflexWetw {
   i32 charge;
   i32 leaped;
};

struct upgr_data_VitalScan {
   i32 target;
   i32 oldtarget;
   str tagstr;
   i32 health;
   i32 oldhealth;
   i32 maxhealth;
   i32 hdelta;
   i32 hdtime;
   k32 x, y, z;
   k64 oangle;
   k64 splitfrac;
   k64 damagfrac;
   i32 split;
   i32 rank;
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

struct upgr_data_RPG_B {
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

struct upgr_data_Subweapons {
   i32 charge;
   i32 shots;
   i32 which;
   i32 have;
};

struct upgr_data_HeadsUpDisp {
   /* marine */
   i32 cr;

   /* informant */
   struct interp_data_scr score;
   struct interp_data_scr health;
   struct interp_data_scr overdrive;
};

struct upgr_data {
   struct upgr_data_Seven7s     seven7s;
   struct upgr_data_Flashlight  flashlight;
   struct upgr_data_HeadsUpDisp headsupdisp;
   struct upgr_data_RPG_B       rpg_b;
   struct upgr_data_Magic       magic;
   struct upgr_data_ReactArmor  reactarmor;
   struct upgr_data_ReflexWetw  reflexwetw;
   struct upgr_data_StealthSys  stealthsys;
   struct upgr_data_Subweapons  subweapons;
   struct upgr_data_VitalScan   vitalscan;
   struct upgr_data_Zoom        zoom;
   struct upgr_data_lolsords    lolsords;
};

struct upgrade {
   anonymous struct shopdef shopdef;

   i32 pclass;
   i32 category;
   i32 perf;
   i32 group;
   i32 requires;
   i32 scoreadd;

   i32 key;

   i32 agroups;
   i32 flags;
};

bool P_Upg_CanActivate(struct upgrade *upgr);
bool P_Upg_Toggle(struct upgrade *upgr);
void P_Upg_SetOwned(struct upgrade *upgr);
i32 P_Upg_CheckReqs(struct upgrade *upgr);

cstr Upgr_EnumToStr(i32 n);

#endif
