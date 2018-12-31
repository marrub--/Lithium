// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#if defined(LITH_X)
// Shorthand and classes. For parsing and headers.
LITH_X(pM, pcl_marine)
LITH_X(pC, pcl_cybermage)
LITH_X(pI, pcl_informant)
LITH_X(pW, pcl_wanderer)
LITH_X(pA, pcl_assassin)
LITH_X(pD, pcl_darklord)
LITH_X(pT, pcl_thoth)
LITH_X(pX, pcl_doubletap)

LITH_X(gO, pcl_outcasts)
LITH_X(gM, pcl_missioners)
LITH_X(gI, pcl_intruders)
LITH_X(gA, pcl_any)
LITH_X(gH, pcl_human)
LITH_X(gN, pcl_nonhuman)
LITH_X(gR, pcl_robot)
#undef LITH_X
#elif !defined(p_player_h)
#define p_player_h

#include "common.h"
#include "p_cbi.h"
#include "p_upgrades.h"
#include "p_data.h"
#include "p_bip.h"
#include "p_log.h"
#include "m_list.h"
#include "p_wepinfo.h"
#include "p_shopdef.h"
#include "p_attrib.h"
#include "items.h"

#include <GDCC/HashMap.h>

#define MAX_PLAYERS 8

#define Lith_ForPlayer() \
   for(i32 _piter = 0; _piter < MAX_PLAYERS; _piter++) \
      __with(struct player *p = &players[_piter];) \
         if(p->active)

#define Lith_GiveAllScore(score, nomul) \
   Lith_ForPlayer() \
      p->giveScore(score, nomul)

#define Lith_GiveAllEXP(amt) \
   Lith_ForPlayer() \
      p->giveEXP(amt)

#define LocalPlayer \
   (ACS_PlayerNumber() < 0 ? nil : &players[ACS_PlayerNumber()])
#define PlayerDiscount(n) (i96)((n) * p->discount)
#define NoPlayer(p) (!(p) || !(p)->active)
#define withplayer(ptr) \
   __with(struct player *p = (ptr);) \
      if(!NoPlayer(p))

// Extern Functions ----------------------------------------------------------|

// state
script void Lith_ResetPlayer(struct player *p);
script void Lith_PlayerLoadData(struct player *p);
script void Lith_PlayerSaveData(struct player *p);
stkcall i32  Lith_PlayerCurWeaponType(struct player *p);
stkcall bool Lith_ButtonPressed(struct player *p, i32 bt);
stkcall optargs(1) bool Lith_SetPlayerVelocity(struct player *p, k32 velx, k32 vely, k32 velz, bool add);
void Lith_ValidatePlayerTID(struct player *p);

// gui
stkcall void Lith_PlayerCloseGUI(struct player *p);
stkcall void Lith_PlayerUseGUI(struct player *p, i32 type);

// score
optargs(1) i96 Lith_GiveScore(struct player *p, i96 score, bool nomul);
stkcall void Lith_TakeScore(struct player *p, i96 score);

// attributes
stkcall void Lith_GiveEXP(struct player *p, u64 amt);

// misc
stkcall struct upgrade *Lith_PlayerGetNamedUpgrade(struct player *p, i32 name);
stkcall bool Lith_PlayerGetUpgradeActive(struct player *p, i32 name);
stkcall char const *Lith_PlayerDiscriminator(i32 pclass);

script void Lith_PlayerUpdateData(struct player *p);

stkcall void Lith_PlayerUpdateStats(struct player *p);

struct player *Lith_GetPlayer(i32 tid, i32 ptr);

// Types ---------------------------------------------------------------------|

GDCC_HashMap_Decl(upgrademap_t, i32, struct upgrade)

enum
{
   #define LITH_X(l, r) l = r,
   #include "p_player.h"
};

enum
{
   pro_nb,
   pro_female,
   pro_male,
   pro_object,
   pro_max,
};

enum
{
   gui_none,
   gui_cbi,
   gui_max
};

// Data that needs to be kept track of between frames.
struct player_delta
{
   // Status
   k32 alpha;
   i96 score;
   u32 maxhealth;
   u32 indialogue;

   // Position
   k32 x, y, z;
   k32 pitch, yaw, roll;

   // Movement
   k32 velx, vely, velz;
   k32 pitchv, yawv;
   k32 forwardv, sidev, upv;
   k32 speedmul;
   k32 jumpboost;

   // Input
   i32  buttons;
   bool scopetoken;
   u32  frozen;
   u32  semifrozen;

   // Attributes
   struct player_attributes attr;
};

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
struct player
{
   // state
   __prop reset         {call: Lith_ResetPlayer(this)}
   __prop loadData      {call: Lith_PlayerLoadData(this)}
   __prop saveData      {call: Lith_PlayerSaveData(this)}
   __prop weapontype    {get:  Lith_PlayerCurWeaponType(this)}
   __prop buttonPressed {call: Lith_ButtonPressed(this)}
   __prop setVel        {call: Lith_SetPlayerVelocity(this)}
   __prop mana          {get:  Lith_CheckActorInventory(->tid, so_MagicAmmo)}
   __prop manamax       {get:  ACS_GetMaxInventory(->tid, so_MagicAmmo)}
   __prop validateTID   {call: Lith_ValidatePlayerTID(this)}
   __prop health {get: GetPropI(->tid, APROP_Health),
                  set: SetPropI(->tid, APROP_Health)}
   __prop setActivator {call: ACS_SetActivator(->tid)}
   __prop getVel       {call: mag2k(->velx, ->vely)}
   __prop grabInput  {get: GetMembI(->tid, sm_GrabInput),
                      set: SetMembI(->tid, sm_GrabInput)}
   __prop onground   {get: GetMembI(->tid, sm_OnGround)}
   __prop waterlevel {get: GetPropI(->tid, APROP_Waterlevel)}

   // inventory
   __prop addItem {call: Lith_PlayerAddItem(this)}

   // score
   __prop giveScore {call: Lith_GiveScore(this)}
   __prop takeScore {call: Lith_TakeScore(this)}

   // attributes
   __prop giveEXP {call: Lith_GiveEXP(this)}

   // log
   __prop logB {call: Lith_LogB(this)}
   __prop logF {call: Lith_LogF(this)}
   __prop logH {call: Lith_LogH(this)}

   // gui
   __prop useGUI   {call: Lith_PlayerUseGUI(this)}
   __prop closeGUI {call: Lith_PlayerCloseGUI(this)}

   // misc
   __prop getUpgr       {call: Lith_PlayerGetNamedUpgrade(this)}
   __prop getUpgrActive {call: Lith_PlayerGetUpgradeActive(this)}
   __prop deliverMail   {call: Lith_DeliverMail(this)}
   __prop bipUnlock     {call: Lith_UnlockBIPPage(->bipptr, __arg, ->pclass)}
   __prop discrim       {get:  Lith_PlayerDiscriminator(->pclass)}
   __prop classname     {get:  GetPropS(->tid, APROP_NameTag)}

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

   // Initialization
   bool wasinit;
   bool active;
   bool dead;
   bool reinit;

   // Info
   i32 tid;
   i32 num;
   u64 ticks;
   str name;
   i32 pclass;
   str pcstr;
   i32 pronoun;
   i32 dlgnum;
   i32 fun;

   // Deltas
   anonymous
   struct player_delta cur;
   struct player_delta old;
   i32 oldhealth;
   i32 oldmana;

   // BIP
   struct bip bip, *bipptr;

   // Upgrades
   struct upgr_data upgrdata;
   struct upgrade   upgrades[UPGR_STATIC_MAX];
   upgrademap_t     upgrademap;

   u32  upgrmax;
   bool upgrinit;

   // Inventory
   struct container inv[8];
   struct container misc;

   struct item *useitem;
   struct item *selitem;
   bool         movitem;
   bool         invinit;

   // HUD
   bool hudenabled;

   list hudstrlist;

   struct loginfo log;

   // Score
   i96 scoreaccum;
   str scoreaccumstr;
   i32 scoreaccumtime;
   k64 scoremul;
   k64 discount;

   // Misc
   i32 decvars[8];

   k32 rage;

   char *notes[16];

   u32  nextstep;
   bool hadinfrared;

   // Input
   char txtbuf[8];
   u32  tbptr;

   // Static data
   u32 spawnhealth;
   k32 jumpheight;
   k32 viewheight;
   str stepnoise;

   // pitch/yaw in precalculated sane radian format
   k64 pitchf;
   k64 yawf;

   // Additive angles
   k64 addpitch;
   k64 addyaw;
   k64 addroll;

   // Damage bob angles
   k64 bobpitch;
   k64 bobyaw;

   // Extra angles
   k64 extrpitch;
   k64 extryaw;

   // View TIDs
   i32 cameratid;
   i32 weathertid;

   // GUI
   u32 activegui;
   struct cbi cbi;

   // Statistics
   u32 healthsum;
   u32 healthused;

   i96 scoresum;
   i96 scoreused;

   u32 unitstravelled;

   u32 spuriousexplosions;
   u32 brouzouf;

   u32 weaponsheld;
   u32 itemsbought;
   u32 upgradesowned;

   // Weapons
   struct weapondata weapon;
   str weaponclass;

   i32  riflefiremode;
   bool autoreload;

   // Keys
   bool krc, kyc, kbc;
   bool krs, kys, kbs;

   // 🌌 「÷」 0
   bool sgacquired;
};

// Extern Objects ------------------------------------------------------------|

extern struct player players[MAX_PLAYERS];

#endif
