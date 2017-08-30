// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#ifndef LITH_WORLD_H
#define LITH_WORLD_H

#include <stdbool.h>

#include "lith_wdata.h"

enum
{
   skill_tourist,
   skill_easy,
   skill_normal,
   skill_hard,
   skill_extrahard,
   skill_nightmare
};

enum
{
   CANONTIME_FULL,
   CANONTIME_SHORT,
   CANONTIME_DATE
};

typedef struct payoutinfo_s
{
   int killnum, killmax;
   int itemnum, itemmax;

   fixed64_t killpct;
   fixed64_t itempct;

   int killscr;
   int itemscr;

   int total;
   int tax;
} payoutinfo_t;

enum game_s
{
   Game_Doom2,
   Game_Episodic,
};

typedef void (*gsinit_cb_t)(void);

__str Lith_CanonTime(int type);
[[__optional_args(1)]] int Lith_UniqueID(int tid);
[[__call("ScriptS"), __extern("ACS")]] void Lith_BeginAngles(int x, int y);
[[__call("ScriptS"), __extern("ACS")]] fixed Lith_AddAngle(int x, int y);
void Lith_FreezeTime(bool on);
void Lith_GSInitRegister(gsinit_cb_t cb);

typedef struct worldinfo_s
{
   attr mapsecrets {get: ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS)}
   attr mapkills   {get: ACS_GetLevelInfo(LEVELINFO_KILLED_MONSTERS)}
   attr mapitems   {get: ACS_GetLevelInfo(LEVELINFO_FOUND_ITEMS)}
   attr mapnum     {get: ACS_GetLevelInfo(LEVELINFO_LEVELNUM)}
   attr mapkillmax {get: ACS_GetLevelInfo(LEVELINFO_TOTAL_MONSTERS)}
   attr mapitemmax {get: ACS_GetLevelInfo(LEVELINFO_TOTAL_ITEMS)}
   attr cluster    {get: ACS_GetLevelInfo(LEVELINFO_CLUSTERNUM)}
   attr canontime      {get: Lith_CanonTime(CANONTIME_FULL)}
   attr canontimeshort {get: Lith_CanonTime(CANONTIME_SHORT)}
   attr canondate      {get: Lith_CanonTime(CANONTIME_DATE)}
   attr difficulty     {get: ACS_GetCVar("lith_sv_difficulty")}
   attr begAngles      {call: Lith_BeginAngles()}
   attr addAngles      {call: Lith_AddAngle()}
   attr freeze         {call: Lith_FreezeTime()}
   attr autosave       {get: ACS_GetCVar("lith_sv_autosave")}

   bool   gsinit;
   bool   singleplayer;
   int    mapscleared;
   int    prevcluster;
   int    mapseed;
   bool   unloaded;
   bool   islithmap;
   bool   enemycompat;
   bool   enemycheck;
   int    secretsfound;
   double scoremul;
   long   ticks;
   int    game;

   // Bosses
   bool boss[3][3];
   bool bossspawned;

   attr boss1p1scr {get: ACS_GetCVar("lith_sv_boss1p1_scr")}
   attr boss1p2scr {get: ACS_GetCVar("lith_sv_boss1p2_scr")}

   attr boss2p1scr {get: ACS_GetCVar("lith_sv_boss2p1_scr")}
   attr boss2p2scr {get: ACS_GetCVar("lith_sv_boss2p2_scr")}
   attr boss2p3scr {get: ACS_GetCVar("lith_sv_boss2p3_scr")}

   attr boss3p1scr {get: ACS_GetCVar("lith_sv_boss3p1_scr")}
   attr boss3p2scr {get: ACS_GetCVar("lith_sv_boss3p2_scr")}
   attr boss3p3scr {get: ACS_GetCVar("lith_sv_boss3p3_scr")}

   // CBI global information
   int  cbiperf;
   bool cbiupgr[cupg_max];

   // Mod compat
   bool legendoom;
   bool drlamonsters;

   // Extras
   bool grafZoneEntered;
   bool pauseinmenus;

   // DECORATE data
   int a_x, a_y;
   struct polar a_angles[8];
   int a_cur;
   int decvars[8];

   // Debugging
   int  dbgLevel;
   bool dbgItems : 1;
   bool dbgBIP   : 1;
   bool dbgScore : 1;
   bool dbgUpgr  : 1;
   bool dbgSave  : 1;
   bool dbgNoMon : 1;
} worldinfo_t;

#ifndef EXTERNAL_CODE
extern bool lmvar mapinit;
extern worldinfo_t world;
#else
worldinfo_t *Lith_GetWorldExtern(void);
#define world (*Lith_GetWorldExtern())
#endif

#endif

