#ifndef LITH_PLAYER_H
#define LITH_PLAYER_H

#include "lith_common.h"
#include "lith_cbi.h"
#include "lith_upgrades.h"
#include "lith_pdata.h"
#include "lith_bip.h"
#include "lith_log.h"
#include "lith_list.h"
#include "lith_weaponinfo.h"
#include "lith_games.h"
#include "lith_shopdef.h"

#include "Lth_hashmap.h"

#define MAX_PLAYERS 8
#define INVALID_PLAYER (&players[-1])

#define Lith_ForPlayer() \
   for(int _piter = 0; _piter < MAX_PLAYERS; _piter++) \
      __with(player_t *p = &players[_piter];) \
         if(p->active)

#define Lith_GiveAllScore(score, nomul) \
   Lith_ForPlayer() \
      p->giveScore(score, nomul)

#define LocalPlayer (&players[ACS_PlayerNumber()])
#define PlayerDiscount(n) (score_t)((n) * p->discount)

#define Lith_GetPCVarInt(p, ...)    ACS_GetUserCVar      ((p)->num, __VA_ARGS__)
#define Lith_GetPCVarFixed(p, ...)  ACS_GetUserCVarFixed ((p)->num, __VA_ARGS__)
#define Lith_GetPCVarString(p, ...) ACS_GetUserCVarString((p)->num, __VA_ARGS__)

#define Lith_SetPCVarInt(p, ...)    ACS_SetUserCVar      ((p)->num, __VA_ARGS__)
#define Lith_SetPCVarFixed(p, ...)  ACS_SetUserCVarFixed ((p)->num, __VA_ARGS__)
#define Lith_SetPCVarString(p, ...) ACS_SetUserCVarString((p)->num, __VA_ARGS__)


//----------------------------------------------------------------------------
// Types
//

enum
{
   pclass_marine,
   pclass_cybermage,
   pclass_max,
   pclass_any = pclass_max,
};

enum
{
   ARM_none,
   ARM_bonus,
   ARM_green,
   ARM_blue,
   ARM_unknown,
   ARM_max
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
   bool    indialogue;
   
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
   int   num;
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
   bip_t bip;
   
   struct upgr_data upgrdata;
   upgrade_t   upgrades[UPGR_STATIC_MAX];
   Lth_HashMap upgrademap;
   int         upgrmax;
   
   loginfo_t loginfo;
   bool      hudenabled;
   
   int decvars[8];
   char txtbuf[8];
   int tbptr;
   
   score_t scoreaccum;
   int     scoreaccumtime;
   double  scoremul;
   float   discount;
   
   int spuriousexplosions;
   int brouzouf;
   bool hadinfrared;
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
   int weathertid;
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
   struct sigil_s
   {
      bool acquired;
   } sigil;
} player_invdata_t;


//----------------------------------------------------------------------------
// Extern Functions
//

void Lith_PlayerCloseGUI(struct player *p);
void Lith_PlayerUseGUI(struct player *p, guiname_t type);

[[__optional_args(1)]]
score_t Lith_GetModScore(struct player *p, score_t score, bool nomul);
[[__optional_args(1)]]
void Lith_GiveScore(struct player *p, score_t score, bool nomul);
void Lith_TakeScore(struct player *p, score_t score);

void Lith_ValidatePlayerTID(struct player *p);

[[__call("ScriptS")]] void Lith_PlayerLoadData(struct player *p);
[[__call("ScriptS")]] void Lith_PlayerSaveData(struct player *p);

[[__call("ScriptS")]] void Lith_PlayerUpdateData(struct player *p);
[[__call("ScriptS")]] void Lith_ResetPlayer(struct player *p);

                      void Lith_PlayerPowerupFx(struct player *p);
[[__call("ScriptS")]] void Lith_PlayerDamageBob(struct player *p);
[[__call("ScriptS")]] void Lith_PlayerView(struct player *p);
                      void Lith_PlayerStyle(struct player *p);
[[__call("ScriptS")]] void Lith_PlayerHUD(struct player *p);
[[__optional_args(2)]]
bool Lith_SetPlayerVelocity(struct player *p, fixed velx, fixed vely, fixed velz, bool add, bool setbob);
bool Lith_ButtonPressed(struct player *p, int bt);
int Lith_PlayerCurWeaponType(struct player *p);
int Lith_PlayerGetClass(struct player *p);

void Lith_PlayerDeltaStats(struct player *p);
upgrade_t *Lith_PlayerGetNamedUpgrade(struct player *p, int name);
void Lith_ClearTextBuf(struct player *p);
struct player *Lith_GetPlayer(int tid, int ptr);

void Lith_PlayerMagicMenu(struct player *p);


//----------------------------------------------------------------------------
// Types
//

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
// edit 11/3/2017: NOW WITH PROPERTY HELL
// edit 11/7/2017: and now it's over 5000 bytes.
// edit 14/7/2017: lol nevermind it's only 2kb now
//
typedef struct player
{
   property reset     {call: Lith_ResetPlayer(this)}
   property loadData  {call: Lith_PlayerLoadData(this)}
   property saveData  {call: Lith_PlayerSaveData(this)}
   property giveScore {call: Lith_GiveScore(this)}
   property takeScore {call: Lith_TakeScore(this)}
   property log       {call: Lith_Log (this)}
   property logF      {call: Lith_LogF(this)}
   property logH      {call: Lith_LogH(this)}
   property setVel    {call: Lith_SetPlayerVelocity(this)}
   property useGUI    {call: Lith_PlayerUseGUI(this)}
   property closeGUI  {call: Lith_PlayerCloseGUI(this)}
   property getUpgr   {call: Lith_PlayerGetNamedUpgrade(this)}
   property buttonPressed {call: Lith_ButtonPressed(this)}
   property weapontype  {get:  Lith_PlayerCurWeaponType(this)}
   property deliverMail {call: Lith_DeliverMail(this)}
   property pclass      {get: Lith_PlayerGetClass(this)}
   property getModScore {call: Lith_GetModScore(this)}
   property clearTextBuf {call: Lith_ClearTextBuf(this)}
   
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

#endif
