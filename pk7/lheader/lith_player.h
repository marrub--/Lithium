// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
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
#include "lith_shopdef.h"

#include <GDCC/HashMap.h>

#define MAX_PLAYERS 8

#define Lith_ForPlayer() \
   for(int _piter = 0; _piter < MAX_PLAYERS; _piter++) \
      __with(player_t *p = &players[_piter];) \
         if(p->active)

#define Lith_GiveAllScore(score, nomul) \
   Lith_ForPlayer() \
      p->giveScore(score, nomul)

#define LocalPlayer \
   (ACS_PlayerNumber() < 0 ? null : &players[ACS_PlayerNumber()])
#define PlayerDiscount(n) (score_t)((n) * p->discount)
#define NoPlayer(p) (!(p) || !(p)->active)
#define withplayer(player) \
   __with(player_t *p = (player);) \
      if(!NoPlayer(p))

// Types ---------------------------------------------------------------------|

GDCC_HashMap_Decl(upgrademap_t, int, upgrade_t)

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
   fixed pitch, yaw, roll;

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
   int  semifrozen;
} player_delta_t;

// Extern Functions ----------------------------------------------------------|

// state
[[__call("ScriptS")]] void Lith_ResetPlayer(struct player *p);
[[__call("ScriptS")]] void Lith_PlayerLoadData(struct player *p);
[[__call("ScriptS")]] void Lith_PlayerSaveData(struct player *p);
                      int  Lith_PlayerCurWeaponType(struct player *p);
                      bool Lith_ButtonPressed(struct player *p, int bt);
                      bool Lith_SetPlayerVelocity(struct player *p,
                         fixed velx, fixed vely, fixed velz,
                         bool add, bool setbob);
                      void Lith_ValidatePlayerTID(struct player *p);

// gui
void Lith_PlayerCloseGUI(struct player *p);
void Lith_PlayerUseGUI(struct player *p, guiname_t type);

// score
[[__optional_args(1)]] void Lith_GiveScore(struct player *p, score_t score,
                          bool nomul);
                       void Lith_TakeScore(struct player *p, score_t score);
[[__optional_args(1)]] score_t Lith_GetModScore(struct player *p,
                          score_t score, bool nomul);

// misc
upgrade_t *Lith_PlayerGetNamedUpgrade(struct player *p, int name);
void Lith_ClearTextBuf(struct player *p);
__str Lith_PlayerDiscriminator(int pclass);

[[__call("ScriptS")]] void Lith_PlayerUpdateData(struct player *p);

void Lith_PlayerDeltaStats(struct player *p);

void Lith_HUD_DrawWeaponSlots(struct player *p, int const *ncol, int ncols,
   char scol, int bx, int by);

struct player *Lith_GetPlayer(int tid, int ptr);

// Types ---------------------------------------------------------------------|

//
// player_t
//
//       7/4/2016: That's a lot of data!
// edit  9/4/2016: Holy shit, that's really a lot of data!
// edit  7/5/2016: JESUS TAKE THE WHEEL
// edit  3/1/2017: help
// edit  6/1/2017: there's so much data that I had to split it
// edit 23/1/2017: D E S T R O Y
// edit 26/2/2017: There is yet again so much data that I had to split it.
// edit 11/3/2017: NOW WITH PROPERTY HELL
// edit 11/7/2017: and now it's over 5000 bytes.
// edit 14/7/2017: lol nevermind it's only 2kb now
// edit 31/8/2017: m e r g e
//
typedef struct player
{
   // Properties -------------------------------------------------------------|

   // state
   attr reset    {call: Lith_ResetPlayer(this)}
   attr loadData {call: Lith_PlayerLoadData(this)}
   attr saveData {call: Lith_PlayerSaveData(this)}
   attr weapontype    {get:  Lith_PlayerCurWeaponType(this)}
   attr buttonPressed {call: Lith_ButtonPressed(this)}
   attr setVel  {call: Lith_SetPlayerVelocity(this)}
   attr mana    {get: Lith_CheckActorInventory(->tid, "Lith_MagicAmmo")}
   attr manamax {get: ACS_GetMaxInventory     (->tid, "Lith_MagicAmmo")}
   attr validateTID {call: Lith_ValidatePlayerTID(this)}

   // score
   attr giveScore   {call: Lith_GiveScore(this)}
   attr takeScore   {call: Lith_TakeScore(this)}
   attr getModScore {call: Lith_GetModScore(this)}

   // log
   attr log  {call: Lith_Log (this)}
   attr logF {call: Lith_LogF(this)}
   attr logH {call: Lith_LogH(this)}

   // gui
   attr useGUI   {call: Lith_PlayerUseGUI(this)}
   attr closeGUI {call: Lith_PlayerCloseGUI(this)}

   // misc
   attr getUpgr      {call: Lith_PlayerGetNamedUpgrade(this)}
   attr deliverMail  {call: Lith_DeliverMail(this)}
   attr clearTextBuf {call: Lith_ClearTextBuf(this)}
   attr bipUnlock {call: Lith_UnlockBIPPage(->bipPtr, __arg, ->pclass)}
   attr discrim   {get:  Lith_PlayerDiscriminator(->pclass)}

   // cvars
   attr getCVarI {call: ACS_GetUserCVar      (->num)}
   attr getCVarK {call: ACS_GetUserCVarFixed (->num)}
   attr getCVarS {call: ACS_GetUserCVarString(->num)}
   attr setCVarI {call: ACS_SetUserCVar      (->num)}
   attr setCVarK {call: ACS_SetUserCVarFixed (->num)}
   attr setCVarS {call: ACS_SetUserCVarString(->num)}

   // shop
   attr getCost {call: Lith_ShopGetCost(this)}
   attr canBuy  {call: Lith_ShopCanBuy(this)}
   attr buy     {call: Lith_ShopBuy(this)}

   // Members ----------------------------------------------------------------|

   // Initialization
   bool wasinit;
   bool active;
   bool dead;
   bool reinit;

   // Info
   int   tid;
   int   num;
   long  ticks;
   __str name;
   int   pclass;

   // Deltas
   [[__anonymous]] player_delta_t cur;
   player_delta_t old;

   // BIP
   bip_t bip, *bipPtr;

   // Upgrades
   struct upgr_data upgrdata;
   upgrade_t        upgrades[UPGR_STATIC_MAX];
   upgrademap_t     upgrademap;
   int              upgrmax;
   bool             upgrinit;

   // HUD
   loginfo_t loginfo;
   bool      hudenabled;

   // DECORATE/ZScript
   int decvars[8];
   char txtbuf[8];
   int tbptr;

   // Score
   score_t scoreaccum;
   int     scoreaccumtime;
   double  scoremul;
   float   discount;

   // Misc
   int spuriousexplosions;
   int brouzouf;
   bool hadinfrared;

   // Static data
   bool  staticinit;
   int   maxhealth;
   int   maxarmor;
   fixed jumpheight;
   fixed viewheight;

   // pitch/yaw in precalculated sane radian format
   float pitchf;
   float yawf;

   // Additive angles
   float addpitch;
   float addyaw;
   float addroll;

   // Damage bob angles
   float bobpitch;
   float bobyaw;

   // Extra angles
   float extrpitch;
   float extryaw;

   // View TIDs
   int cameratid;
   int weathertid;

   // GUI
   guiname_t activegui;
   cbi_t     cbi;

   // Statistics
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

   // Weapons
   weapondata_t weapon;

   int    riflefiremode;
   list_t hudstrlist;

   __str weaponclass;

   // Armor
   __str armorclass;
   int armortype;

   // Keys
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
   struct divsigil
   {
      bool acquired;
   } sigil;
} player_t;

// Extern Objects ------------------------------------------------------------|

#ifndef EXTERNAL_CODE
extern player_t players[MAX_PLAYERS];
#else
player_t (*Lith_GetPlayersExtern(void))[MAX_PLAYERS];
#define players (*Lith_GetPlayersExtern())
#endif

#endif
