// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#ifndef LITH_WORLD_H
#define LITH_WORLD_H

#include <stdbool.h>

#include "lith_wdata.h"

#define InSecret \
   (world.game == Game_Doom2 && (world.cluster == 9 || world.cluster == 10))

#define InHell \
   (world.game == Game_Doom2 && world.cluster >= 8)

#define OnEarth \
   (world.game == Game_Doom2 && world.cluster == 7)

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

   fixed64 killpct;
   fixed64 itempct;

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

__str Lith_CanonTime(int type);
optargs(1) int Lith_UniqueID(int tid);
stkcall void Lith_BeginAngles(int x, int y);
stkcall fixed Lith_AddAngle(int x, int y);
stkcall void Lith_FreezeTime(bool on);

typedef struct worldinfo_s
{
   __prop mapsecrets {get: ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS)}
   __prop mapkills   {get: ACS_GetLevelInfo(LEVELINFO_KILLED_MONSTERS)}
   __prop mapitems   {get: ACS_GetLevelInfo(LEVELINFO_FOUND_ITEMS)}
   __prop mapnum     {get: ACS_GetLevelInfo(LEVELINFO_LEVELNUM)}
   __prop mapkillmax {get: ACS_GetLevelInfo(LEVELINFO_TOTAL_MONSTERS)}
   __prop mapitemmax {get: ACS_GetLevelInfo(LEVELINFO_TOTAL_ITEMS)}
   __prop cluster    {get: ACS_GetLevelInfo(LEVELINFO_CLUSTERNUM)}
   __prop canontime      {get: Lith_CanonTime(CANONTIME_FULL)}
   __prop canontimeshort {get: Lith_CanonTime(CANONTIME_SHORT)}
   __prop canondate      {get: Lith_CanonTime(CANONTIME_DATE)}
   __prop difficulty     {get: ACS_GetCVar("lith_sv_difficulty")}
   __prop begAngles      {call: Lith_BeginAngles()}
   __prop addAngles      {call: Lith_AddAngle()}
   __prop freeze         {call: Lith_FreezeTime()}
   __prop autosave       {get: ACS_GetCVar("lith_sv_autosave")}

   bool    gsinit;
   bool    singleplayer;
   int     mapscleared;
   int     prevcluster;
   int     mapseed;
   bool    unloaded;
   bool    islithmap;
   bool    enemycompat;
   bool    enemycheck;
   int     secretsfound;
   fixed64 scoremul;
   u64     ticks;
   int     game;
   fixed   apiversion;
   int     soulsfreed;

   int fun;

   // Bosses
   bool bossspawned;

   // CBI global information
   int  cbiperf;
   bool cbiupgr[cupg_max];

   // Mod compat
   bool legendoom;
   bool drlamonsters;

   // Extras
   bool pauseinmenus;

   // DECORATE data
   int a_x, a_y;
   struct polar a_angles[8];
   int a_cur;
   int decvars[8];

   // Debugging
   int  dbgLevel;
   bool dbgItems;
   bool dbgBIP;
   bool dbgScore;
   bool dbgUpgr;
   bool dbgSave;
   bool dbgNoMon;
} worldinfo_t;

#ifndef EXTERNAL_CODE
extern bool lmvar mapinit;
extern worldinfo_t world;
extern payoutinfo_t payout;
#else
worldinfo_t *Lith_GetWorldExtern(void);
#define world (*Lith_GetWorldExtern())
#endif

#endif

