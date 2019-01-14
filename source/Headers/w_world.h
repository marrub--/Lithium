// Distributed under the CC0 public domain license.
// By Alison Sanderson. Attribution is encouraged, though not required.
// See licenses/cc0.txt for more information.

// w_world.h: World data.

#ifndef w_world_h
#define w_world_h

#include <stdbool.h>

#include "w_data.h"

#define InSecret \
   (world.game == Game_Doom2 && (Cluster == 9 || Cluster == 10))
#define InHell \
   (world.game == Game_Doom2 && Cluster >= 8)
#define OnEarth \
   (world.game == Game_Doom2 && Cluster == 7)

#define MapNum ACS_GetLevelInfo(LEVELINFO_LEVELNUM)
#define Cluster ACS_GetLevelInfo(LEVELINFO_CLUSTERNUM)

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
   ct_full,
   ct_short,
   ct_date
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

char const *CanonTime(i32 type);
optargs(1) i32 UniqueID(i32 tid);
stkcall void BeginAngles(i32 x, i32 y);
stkcall k32 AddAngle(i32 x, i32 y);
stkcall void FreezeTime(bool on);
script void CBI_Install(i32 num);

struct worldinfo
{
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
   i32  fun;
   bool bossspawned;
   i32  cbiperf;
   bool cbiupgr[cupg_max];
   bool legendoom;
   bool drlamonsters;
   bool pauseinmenus;

   // DECORATE data
   i32 a_x, a_y;
   struct polar a_angles[8];
   i32 a_cur;
};

extern bool lmvar player_init;
extern struct worldinfo world;
extern struct payoutinfo payout;

#endif
