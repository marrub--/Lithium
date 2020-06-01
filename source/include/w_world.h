/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
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

#define InSecret (Cluster == 9 || Cluster == 10)
#define InHell   (Cluster >= 8)
#define OnEarth  (Cluster == 7)

#define MapNum  ACS_GetLevelInfo(LEVELINFO_LEVELNUM)
#define Cluster ACS_GetLevelInfo(LEVELINFO_CLUSTERNUM)

/* Line 1888300 is used as a control line for mod features.
 * Similarly, maps in a certain range are marked as Lithium maps.
 */
#define LithMapLine 0x001CD02C
#define LithMapBeg  0x001CCF00
#define LithMapEnd  0x001CD2E7

#define GetFun()  ACS_GetCVar(sc_fun)
#define SetFun(x) ACS_SetCVar(sc_fun, x)

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
   i32 killnum, killmax, killscr;
   i32 itemnum, itemmax, itemscr;
   i32 scrtnum, scrtmax, scrtscr;

   k64 killpct, itempct, scrtpct;

   i32 total;
   i32 tax;

   i32 par;

   struct {
      i32 kill100;
      i32 item100;
      i32 scrt100;
      i32 par;
      i32 sponsor;
   } activities;
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
extern bool lmvar islithmap;
extern i32 secretsfound;
extern k64 scoremul;
extern u64 ticks;
extern k32 apiversion;
extern i32 soulsfreed;
extern bool bossspawned;
extern i32 cbiperf;
extern bool cbiupgr[cupg_max];
extern bool legendoom;
extern bool drlamonsters;
extern enum mission_status lmvar mission;
extern bool dorain;

/* DECORATE data */
extern i32 a_x, a_y;
extern struct polar a_angles[8];
extern i32 a_cur;

/* Extern Functions -------------------------------------------------------- */

void Scr_HInit(void);
cstr CanonTime(i32 type, u64 time);
optargs(1) i32 UniqueID(i32 tid);
void BeginAngles(i32 x, i32 y);
k32 AddAngle(i32 x, i32 y);
optargs(1) void FreezeTime(bool players_ok);
optargs(1) void UnfreezeTime(bool players_ok);
script void CBI_Install(i32 num);
script void W_Title(void);
script void F_Run(struct player *p);
void F_Load(void);
void F_Start(cstr which);
script void W_DoRain();

#endif
