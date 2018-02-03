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
#include "lith_items.h"
#include "lith_hud.h"

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
   pro_none,
   pro_male,
   pro_female,
   pro_object,
   pro_max,
};

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
   int     maxhealth;
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

   // Attributes
   struct player_attributes attr;
} player_delta_t;

// Extern Functions ----------------------------------------------------------|

// state
[[__call("ScriptS")]]  void Lith_ResetPlayer(struct player *p);
[[__call("ScriptS")]]  void Lith_PlayerLoadData(struct player *p);
[[__call("ScriptS")]]  void Lith_PlayerSaveData(struct player *p);
                       int  Lith_PlayerCurWeaponType(struct player *p);
                       bool Lith_ButtonPressed(struct player *p, int bt);
[[__optional_args(1)]] bool Lith_SetPlayerVelocity(struct player *p,
                          fixed velx, fixed vely, fixed velz, bool add);
                       void Lith_ValidatePlayerTID(struct player *p);

// gui
void Lith_PlayerCloseGUI(struct player *p);
void Lith_PlayerUseGUI(struct player *p, guiname_t type);

// score
[[__optional_args(1)]] score_t Lith_GiveScore(struct player *p, score_t score, bool nomul);
void Lith_TakeScore(struct player *p, score_t score);

// attributes
void Lith_GiveEXP(struct player *p, unsigned long amt);

// misc
upgrade_t *Lith_PlayerGetNamedUpgrade(struct player *p, int name);
bool Lith_PlayerGetUpgradeActive(struct player *p, int name);
void Lith_ClearTextBuf(struct player *p);
__str Lith_PlayerDiscriminator(int pclass);

[[__call("ScriptS")]] void Lith_PlayerUpdateData(struct player *p);

void Lith_PlayerUpdateStats(struct player *p);

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
   __prop reset         {call: Lith_ResetPlayer(this)}
   __prop loadData      {call: Lith_PlayerLoadData(this)}
   __prop saveData      {call: Lith_PlayerSaveData(this)}
   __prop weapontype    {get:  Lith_PlayerCurWeaponType(this)}
   __prop buttonPressed {call: Lith_ButtonPressed(this)}
   __prop setVel        {call: Lith_SetPlayerVelocity(this)}
   __prop mana          {get:  Lith_CheckActorInventory(->tid, "Lith_MagicAmmo")}
   __prop manamax       {get:  ACS_GetMaxInventory(->tid, "Lith_MagicAmmo")}
   __prop validateTID   {call: Lith_ValidatePlayerTID(this)}
   __prop health {get: ACS_GetActorProperty(->tid, APROP_Health),
                  set: ACS_SetActorProperty(->tid, APROP_Health)}
   __prop setActivator {call: ACS_SetActivator(->tid)}
   __prop getVel {call: mag2k(->velx, ->vely)}

   // inventory
   __prop addItem {call: Lith_PlayerAddItem(this)}

   // score
   __prop giveScore {call: Lith_GiveScore(this)}
   __prop takeScore {call: Lith_TakeScore(this)}

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
   __prop bipUnlock     {call: Lith_UnlockBIPPage(->bipptr, __arg, ->pclass)}
   __prop discrim       {get:  Lith_PlayerDiscriminator(->pclass)}
   __prop classname     {get:  ACS_GetActorPropertyString(->tid, APROP_NameTag)}

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
   __str pcstr;
   int   pronoun;

   // Deltas
   [[__anonymous]] player_delta_t cur;
   player_delta_t old;
   int oldhealth;

   // BIP
   bip_t bip, *bipptr;

   // Upgrades
   struct upgr_data upgrdata;
   upgrade_t        upgrades[UPGR_STATIC_MAX];
   upgrademap_t     upgrademap;
   int              upgrmax;
   bool             upgrinit;

   // Inventory
   container_t inv[8];
   container_t misc;
   item_t     *useitem;
   item_t     *selitem;
   bool        movitem;

   // HUD
   loginfo_t  loginfo;
   bool       hudenabled;
   struct hud hud;

   // DECORATE/ZScript
   int  decvars[8];
   char txtbuf[8];
   int  tbptr;

   // Score
   score_t scoreaccum;
   int     scoreaccumtime;
   double  scoremul;
   float   discount;

   // Misc
   int   spuriousexplosions;
   int   brouzouf;
   bool  hadinfrared;
   fixed rage;
   __str notes[16];
   bool  autoreload;

   // Static data
   bool  staticinit;
   int   spawnhealth;
   fixed spawndfactor;
   fixed jumpheight;
   fixed viewheight;
   __str stepnoise;

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

   score_t scoresum;
   score_t scoreused;

   int unitstravelled;

   // Weapons
   weapondata_t weapon;

   int    riflefiremode;
   list_t hudstrlist;

   __str weaponclass;

   // Keys
   struct keycards_s
   {
      bool rc : 1;
      bool yc : 1;
      bool bc : 1;
      bool rs : 1;
      bool ys : 1;
      bool bs : 1;
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

// Called when an alive player updates. Cleared before GInit is called.
CallbackDeclareInternal(player_cb_t, PlayerUpdate)
// Called when a player finishes updating. Cleared before GInit is called.
CallbackDeclareInternal(player_cb_t, PlayerRender)

#endif
