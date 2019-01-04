// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

#ifndef w_world_h
#define w_world_h

#include <stdbool.h>

#include "w_data.h"

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

struct payoutinfo
{
   i32 killnum, killmax;
   i32 itemnum, itemmax;

   k64 killpct;
   k64 itempct;

   i32 killscr;
   i32 itemscr;

   i32 total;
   i32 tax;
};

enum game
{
   Game_Doom2,
   Game_Episodic,
};

char const *Lith_CanonTime(i32 type);
optargs(1) i32 Lith_UniqueID(i32 tid);
stkcall void Lith_BeginAngles(i32 x, i32 y);
stkcall k32 Lith_AddAngle(i32 x, i32 y);
stkcall void Lith_FreezeTime(bool on);
script void Lith_InstallCBIItem(i32 num);

struct worldinfo
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
   __prop difficulty     {get: ACS_GetCVar(sc_sv_difficulty)}
   __prop begAngles      {call: Lith_BeginAngles()}
   __prop addAngles      {call: Lith_AddAngle()}
   __prop freeze         {call: Lith_FreezeTime()}
   __prop autosave       {get: ACS_GetCVar(sc_sv_autosave)}

   bool gsinit;
   bool singleplayer;
   i32  mapscleared;
   i32  prevcluster;
   i32  mapseed;
   bool unloaded;
   bool islithmap;
   bool enemycompat;
   bool enemycheck;
   i32  secretsfound;
   k64  scoremul;
   u64  ticks;
   i32  game;
   k32  apiversion;
   i32  soulsfreed;

   i32 fun;

   // Bosses
   bool bossspawned;

   // CBI global information
   i32  cbiperf;
   bool cbiupgr[cupg_max];

   // Mod compat
   bool legendoom;
   bool drlamonsters;

   // Extras
   bool pauseinmenus;

   // DECORATE data
   i32 a_x, a_y;
   struct polar a_angles[8];
   i32 a_cur;
   i32 decvars[8];

   // Debugging
   i32  dbgLevel;
   bool dbgItems;
   bool dbgBIP;
   bool dbgScore;
   bool dbgUpgr;
   bool dbgSave;
   bool dbgNoMon;
};

extern bool lmvar mapinit;
extern struct worldinfo world;
extern struct payoutinfo payout;

#endif
