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
#include "lith_attrib.h"

#include <GDCC/HashMap.h>

#define MAX_PLAYERS 8

#define Lith_ForPlayer() \
   for(int _piter = 0; _piter < MAX_PLAYERS; _piter++) \
      __with(player_t *p = &players[_piter];) \
         if(p->active)

#define Lith_GiveAllScore(score, nomul) \
   Lith_ForPlayer() \
      p->giveScore(score, nomul)

#define Lith_GiveAllEXP(amt) \
   Lith_ForPlayer() \
      p->giveEXP(amt)

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
   fixed   alpha;
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

// attributes
void Lith_GiveEXP(struct player *p, unsigned long amt);

// misc
upgrade_t *Lith_PlayerGetNamedUpgrade(struct player *p, int name);
bool Lith_PlayerGetUpgradeActive(struct player *p, int name);
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
   __prop reset    {call: Lith_ResetPlayer(this)}
   __prop loadData {call: Lith_PlayerLoadData(this)}
   __prop saveData {call: Lith_PlayerSaveData(this)}
   __prop weapontype    {get:  Lith_PlayerCurWeaponType(this)}
   __prop buttonPressed {call: Lith_ButtonPressed(this)}
   __prop setVel  {call: Lith_SetPlayerVelocity(this)}
   __prop mana    {get: Lith_CheckActorInventory(->tid, "Lith_MagicAmmo")}
   __prop manamax {get: ACS_GetMaxInventory     (->tid, "Lith_MagicAmmo")}
   __prop validateTID {call: Lith_ValidatePlayerTID(this)}

   // score
   __prop giveScore   {call: Lith_GiveScore(this)}
   __prop takeScore   {call: Lith_TakeScore(this)}
   __prop getModScore {call: Lith_GetModScore(this)}

   // attributes
   __prop giveEXP {call: Lith_GiveEXP(this)}

   // log
   __prop log  {call: Lith_Log (this)}
   __prop logF {call: Lith_LogF(this)}
   __prop logH {call: Lith_LogH(this)}

   // gui
   __prop useGUI   {call: Lith_PlayerUseGUI(this)}
   __prop closeGUI {call: Lith_PlayerCloseGUI(this)}

   // misc
   __prop getUpgr       {call: Lith_PlayerGetNamedUpgrade(this)}
   __prop getUpgrActive {call: Lith_PlayerGetUpgradeActive(this)}
   __prop deliverMail   {call: Lith_DeliverMail(this)}
   __prop clearTextBuf  {call: Lith_ClearTextBuf(this)}
   __prop bipUnlock {call: Lith_UnlockBIPPage(->bipptr, __arg, ->pclass)}
   __prop discrim   {get:  Lith_PlayerDiscriminator(->pclass)}
   __prop classname {get:  ACS_GetActorPropertyString(->tid, APROP_NameTag)}

   // cvars
   __prop getCVarI {call: ACS_GetUserCVar      (->num)}
   __prop getCVarK {call: ACS_GetUserCVarFixed (->num)}
   __prop getCVarS {call: ACS_GetUserCVarString(->num)}
   __prop setCVarI {call: ACS_SetUserCVar      (->num)}
   __prop setCVarK {call: ACS_SetUserCVarFixed (->num)}
   __prop setCVarS {call: ACS_SetUserCVarString(->num)}

   // shop
   __prop getCost {call: Lith_ShopGetCost(this)}
   __prop canBuy  {call: Lith_ShopCanBuy(this)}
   __prop buy     {call: Lith_ShopBuy(this)}

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

   // Attributes
   struct player_attributes attr;

   // BIP
   bip_t bip, *bipptr;

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

typedef void (*player_cb_t)(player_t *p);

// Extern Objects ------------------------------------------------------------|

#ifndef EXTERNAL_CODE
extern player_t players[MAX_PLAYERS];
#else
player_t (*Lith_GetPlayersExtern(void))[MAX_PLAYERS];
#define players (*Lith_GetPlayersExtern())
#endif

// Callbacks -----------------------------------------------------------------|

CallbackDeclareInternal(player_cb_t, PlayerUpdate)

#endif
