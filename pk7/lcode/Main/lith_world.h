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

__str Lith_CanonTime(int type);

enum game_s
{
   Game_Doom2,
   Game_Episodic,
};

typedef struct worldinfo_s
{
   property mapsecrets {get: ACS_GetLevelInfo(LEVELINFO_FOUND_SECRETS)}
   property mapkills   {get: ACS_GetLevelInfo(LEVELINFO_KILLED_MONSTERS)}
   property mapitems   {get: ACS_GetLevelInfo(LEVELINFO_FOUND_ITEMS)}
   property mapnum     {get: ACS_GetLevelInfo(LEVELINFO_LEVELNUM)}
   property mapkillmax {get: ACS_GetLevelInfo(LEVELINFO_TOTAL_MONSTERS)}
   property mapitemmax {get: ACS_GetLevelInfo(LEVELINFO_TOTAL_ITEMS)}
   property cluster    {get: ACS_GetLevelInfo(LEVELINFO_CLUSTERNUM)}
   property canontime      {get: Lith_CanonTime(CANONTIME_FULL)}
   property canontimeshort {get: Lith_CanonTime(CANONTIME_SHORT)}
   property canondate      {get: Lith_CanonTime(CANONTIME_DATE)}
   
   bool singleplayer;
   int mapscleared;
   int prevcluster;
   int mapseed;
   bool unloaded;
   bool islithmap;
   
   int secretsfound;
   double scoremul;
   
   long ticks;
   
   bool scoregolf;
   
   int game;
   
   // Bosses
   bool boss1p1, boss1p2;
   bool boss2p1, boss2p2, boss2p3;
   bool boss3p1, boss3p2, boss3p3;
   bool bossspawned;
   
   property boss1p1scr {get: ACS_GetCVar("lith_sv_boss1p1_scr")}
   property boss1p2scr {get: ACS_GetCVar("lith_sv_boss1p2_scr")}
   
   property boss2p1scr {get: ACS_GetCVar("lith_sv_boss2p1_scr")}
   property boss2p2scr {get: ACS_GetCVar("lith_sv_boss2p2_scr")}
   property boss2p3scr {get: ACS_GetCVar("lith_sv_boss2p3_scr")}
   
   property boss3p1scr {get: ACS_GetCVar("lith_sv_boss3p1_scr")}
   property boss3p2scr {get: ACS_GetCVar("lith_sv_boss3p2_scr")}
   property boss3p3scr {get: ACS_GetCVar("lith_sv_boss3p3_scr")}
   
   // CBI global information
   struct
   {
      int  perf;
      bool armorinter : 1;
      bool weapninter : 1;
      bool weapninte2 : 1;
      bool rdistinter : 1;
      bool hasupgr1   : 1;
      bool hasupgr2   : 1;
   } cbi;
   
   // Mod compat
   bool legendoom;
   bool drlamonsters;
   
   // Extras
   bool grafZoneEntered;
   bool pauseinmenus;
} worldinfo_t;

__addrdef extern __mod_arr lmvar;
__addrdef extern __hub_arr lwvar;

extern bool lmvar mapinit;

extern worldinfo_t world;
extern payoutinfo_t payout;

[[__optional_args(1)]] int Lith_UniqueID(int tid);

#endif

