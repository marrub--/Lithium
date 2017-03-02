#ifndef LITH_PLAYER_H
#define LITH_PLAYER_H

#include "lith_cbi.h"
#include "lith_upgrades.h"
#include "lith_pdata.h"
#include "lith_sigil.h"
#include "lith_bip.h"
#include "lith_log.h"
#include "lith_list.h"
#include "lith_weaponinfo.h"
#include "lith_games.h"
#include "lith_shopdef.h"

#define MAX_PLAYERS 8
#define HUDSTRS_MAX 20

#define ForPlayer() \
   for(int _piter = 0; _piter < MAX_PLAYERS; _piter++) \
      __with(player_t *p = &players[_piter];) \
         if(p->active)

#define Lith_GiveAllScore(score, nomul) \
   ForPlayer() \
      Lith_GiveScore(p, score, nomul)

#define LocalPlayer (&players[ACS_PlayerNumber()])
#define PlayerDiscount(n) (score_t)((n) * p->discount)


//----------------------------------------------------------------------------
// Types
//

enum
{
   armor_none,
   armor_bonus,
   armor_green,
   armor_blue,
   armor_unknown,
   armor_max
};

//
// guiname_t
//
typedef enum guiname_s
{
   GUI_NONE,
   GUI_CBI,
   GUI_GB,
   GUI_MAX
} guiname_t;

//
// player_delta_t
//
// Data that needs to be kept track of between frames.
//
typedef struct player_delta_s
{
   // Status
   int     health;
   int     armor;
   score_t score;
   
   // Position
   fixed x, y, z;
   fixed floorz;
   fixed pitch, yaw;
   
   // Movement
   fixed velx, vely, velz;
   fixed pitchv, yawv;
   fixed forwardv, sidev, upv;
   fixed speedmul;
   fixed jumpboost;
   
   // Input
   int  buttons;
   bool scopetoken;
   int  frozen;
} player_delta_t;

//
// player_statedata_t
//
typedef struct player_statedata_s
{
   bool active;
   bool dead;
   bool reinit;
   
   int   tid;
   int   number;
   long  ticks;
   __str name;
   
   [[__anonymous]] player_delta_t cur;
   player_delta_t old;
} player_statedata_t;

//
// player_extdata_t
//
typedef struct player_extdata_s
{
   float      discount;
   bip_t      bip;
   upgrades_t upgrades;
   loginfo_t  loginfo;
   
   score_t scoreaccum;
   int     scoreaccumtime;
   double  scoremul;
} player_extdata_t;

//
// player_staticdata_t
//
typedef struct player_staticdata_s
{
   bool  staticinit;
   int   maxhealth;
   int   maxarmor;
   fixed jumpheight;
   fixed viewheight;
} player_staticdata_t;

//
// player_viewdata_t
//
typedef struct player_viewdata_s
{
   // pitch/yaw in precalculated sane radian format
   float pitchf;
   float yawf;
   
   float addpitch;
   float addyaw;
   
   float bobpitch;
   float bobyaw;
   
   float extrpitch;
   float extryaw;
   
   int cameratid;
} player_viewdata_t;

//
// player_guidata_t
//
typedef struct player_guidata_s
{
   guiname_t activegui;
   cbi_t     cbi;
   gb_t      gb;
} player_guidata_t;

//
// player_statdata_t
//
typedef struct player_statdata_s
{
   int weaponsheld;
   int itemsbought;
   int upgradesowned;
   
   long healthsum;
   long healthused;
   
   long armorsum;
   long armorused;
   
   score_t scoresum;
   score_t scoreused;
   
   int unitstravelled;
} player_statdata_t;

//
// player_invdata_t
//
typedef struct player_invdata_s
{
   weapondata_t weapon;
   
   int    riflefiremode;
   list_t hudstrlist;
   
   __str weaponclass;
   __str armorclass;
   
   int armortype;
   
   struct keycards_s
   {
      bool redcard     : 1;
      bool yellowcard  : 1;
      bool bluecard    : 1;
      bool redskull    : 1;
      bool yellowskull : 1;
      bool blueskull   : 1;
   } keys;
   
   // 🌌 「÷」 0
   // sigil_t sigil;
} player_invdata_t;

//
// player_t
//
// 7/4/2016: That's a lot of data!
// edit 9/4/2016: Holy shit, that's really a lot of data!
// edit 7/5/2016: JESUS TAKE THE WHEEL
// edit 3/1/2017: help
// edit 6/1/2017: there's so much data that I had to split it
// edit 23/1/2017: D E S T R O Y
// edit 26/2/2017: There is yet again so much data that I had to split it.
//
typedef struct player_s
{
   [[__anonymous]] player_statedata_t  statedata;
   [[__anonymous]] player_extdata_t    extdata;
   [[__anonymous]] player_staticdata_t staticdata;
   [[__anonymous]] player_viewdata_t   viewdata;
   [[__anonymous]] player_guidata_t    guidata;
   [[__anonymous]] player_statdata_t   statdata;
   [[__anonymous]] player_invdata_t    invdata;
} player_t;


//----------------------------------------------------------------------------
// Extern Objects
//

extern player_t players[MAX_PLAYERS];


//----------------------------------------------------------------------------
// Extern Functions
//

void Lith_PlayerCloseGUI(player_t *p);
void Lith_PlayerUseGUI(player_t *p, guiname_t type);

[[__optional_args(1)]]
void Lith_GiveScore(player_t *p, score_t score, bool nomul);
void Lith_TakeScore(player_t *p, score_t score);

void Lith_ValidatePlayerTID(player_t *p);

[[__call("ScriptS")]] void Lith_PlayerLoadData(player_t *p);
[[__call("ScriptS")]] void Lith_PlayerSaveData(player_t *p);

[[__call("ScriptS")]] void Lith_PlayerUpdateData(player_t *p);
[[__call("ScriptS")]] void Lith_ResetPlayer(player_t *p);

[[__call("ScriptS")]] void Lith_PlayerDamageBob(player_t *p);
[[__call("ScriptS")]] void Lith_PlayerView(player_t *p);
                      void Lith_PlayerStyle(player_t *p);
[[__call("ScriptS")]] void Lith_PlayerHUD(player_t *p);

#endif
