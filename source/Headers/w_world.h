/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * World data.
 *
 * ---------------------------------------------------------------------------|
 */

#ifndef w_world_h
#define w_world_h

#include <stdbool.h>

#include "w_data.h"

#define InSecret (game == Game_Doom2 && (Cluster == 9 || Cluster == 10))
#define InHell   (game == Game_Doom2 && Cluster >= 8)
#define OnEarth  (game == Game_Doom2 && Cluster == 7)

#define MapNum ACS_GetLevelInfo(LEVELINFO_LEVELNUM)
#define Cluster ACS_GetLevelInfo(LEVELINFO_CLUSTERNUM)

#define LithMapMagic 0x01202000
#define LithMapMask  0xFFFFFC00

/* Types ------------------------------------------------------------------- */

enum {
   skill_tourist,
   skill_easy,
   skill_normal,
   skill_hard,
   skill_extrahard,
   skill_nightmare
};

enum {
   ct_full,
   ct_short,
   ct_date
};

struct payoutinfo {
   i32 killnum, killmax;
   i32 itemnum, itemmax;

   k64 killpct;
   k64 itempct;

   i32 killscr;
   i32 itemscr;

   i32 total;
   i32 tax;
};

enum game {
   Game_Doom2,
   Game_Episodic,
};

enum mission_status {
   _unfinished,
   _finished,
   _failure
};

/* Extern Objects ---------------------------------------------------------- */

extern bool lmvar player_init;
extern struct payoutinfo payout;
extern bool singleplayer;
extern i32 mapscleared;
extern i32 prevcluster;
extern i32 mapseed;
extern bool unloaded;
extern bool islithmap;
extern i32 secretsfound;
extern k64 scoremul;
extern u64 ticks;
extern i32 game;
extern k32 apiversion;
extern i32 soulsfreed;
extern i32 fun;
extern bool bossspawned;
extern i32 cbiperf;
extern bool cbiupgr[cupg_max];
extern bool legendoom;
extern bool drlamonsters;
extern bool pauseinmenus;
extern enum mission_status lmvar mission;

/* DECORATE data */
extern i32 a_x, a_y;
extern struct polar a_angles[8];
extern i32 a_cur;

/* Extern Functions -------------------------------------------------------- */

cstr CanonTime(i32 type);
optargs(1) i32 UniqueID(i32 tid);
stkcall void BeginAngles(i32 x, i32 y);
stkcall k32 AddAngle(i32 x, i32 y);
stkcall void FreezeTime(bool on);
script void CBI_Install(i32 num);

#endif
