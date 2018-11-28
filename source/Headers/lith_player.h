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

LITH_X(gO, pcl_outcasts)
LITH_X(gM, pcl_missioners)
LITH_X(gI, pcl_intruders)
LITH_X(gA, pcl_any)
LITH_X(gH, pcl_human)
LITH_X(gN, pcl_nonhuman)
LITH_X(gR, pcl_robot)
#undef LITH_X
#elif !defined(LITH_PLAYER_H)
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

#include <GDCC/HashMap.h>

#define MAX_PLAYERS 8

#define Lith_ForPlayer() \
   for(int _piter = 0; _piter < MAX_PLAYERS; _piter++) \
      __with(struct player *p = &players[_piter];) \
         if(p->active)

#define Lith_GiveAllScore(score, nomul) \
   Lith_ForPlayer() \
      p->giveScore(score, nomul)

#define Lith_GiveAllEXP(amt) \
   Lith_ForPlayer() \
      p->giveEXP(amt)

#define LocalPlayer \
   (ACS_PlayerNumber() < 0 ? null : &players[ACS_PlayerNumber()])
#define PlayerDiscount(n) (i96)((n) * p->discount)
#define NoPlayer(p) (!(p) || !(p)->active)
#define withplayer(ptr) \
   __with(struct player *p = (ptr);) \
      if(!NoPlayer(p))

// Types ---------------------------------------------------------------------|

GDCC_HashMap_Decl(upgrademap_t, int, upgrade_t)

enum
{
#define LITH_X(l, r) l = r,
#include "lith_player.h"
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
   GUI_NONE,
   GUI_CBI,
   GUI_MAX
};

// Data that needs to be kept track of between frames.
struct player_delta
{
   // Status
   fixed alpha;
   i96   score;
   uint  maxhealth;
   uint  indialogue;

   // Position
   fixed x, y, z;
   fixed pitch, yaw, roll;

   // Movement
   fixed velx, vely, velz;
   fixed pitchv, yawv;
   fixed forwardv, sidev, upv;
   fixed speedmul;
   fixed jumpboost;

   // Input
   int buttons;
   bool scopetoken;
   uint frozen;
   uint semifrozen;

   // Attributes
   struct player_attributes attr;
};

// Extern Functions ----------------------------------------------------------|

// state
script void Lith_ResetPlayer(struct player *p);
script void Lith_PlayerLoadData(struct player *p);
script void Lith_PlayerSaveData(struct player *p);
stkcall int  Lith_PlayerCurWeaponType(struct player *p);
stkcall bool Lith_ButtonPressed(struct player *p, int bt);
stkcall optargs(1) bool Lith_SetPlayerVelocity(struct player *p, fixed velx, fixed vely, fixed velz, bool add);
void Lith_ValidatePlayerTID(struct player *p);

// gui
stkcall void Lith_PlayerCloseGUI(struct player *p);
stkcall void Lith_PlayerUseGUI(struct player *p, int type);

// score
optargs(1) i96 Lith_GiveScore(struct player *p, i96 score, bool nomul);
stkcall void Lith_TakeScore(struct player *p, i96 score);

// attributes
stkcall void Lith_GiveEXP(struct player *p, u64 amt);

// misc
stkcall upgrade_t *Lith_PlayerGetNamedUpgrade(struct player *p, int name);
stkcall bool Lith_PlayerGetUpgradeActive(struct player *p, int name);
stkcall char const *Lith_PlayerDiscriminator(int pclass);

script void Lith_PlayerUpdateData(struct player *p);

stkcall void Lith_PlayerUpdateStats(struct player *p);

struct player *Lith_GetPlayer(int tid, int ptr);

// Types ---------------------------------------------------------------------|

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
   __prop mana          {get:  Lith_CheckActorInventory(->tid, "Lith_MagicAmmo")}
   __prop manamax       {get:  ACS_GetMaxInventory(->tid, "Lith_MagicAmmo")}
   __prop validateTID   {call: Lith_ValidatePlayerTID(this)}
   __prop health {get: GetPropI(->tid, APROP_Health),
                  set: SetPropI(->tid, APROP_Health)}
   __prop setActivator {call: ACS_SetActivator(->tid)}
   __prop getVel       {call: mag2k(->velx, ->vely)}
   __prop grabInput  {get: GetMembI(->tid, "m_grabInput"),
                      set: SetMembI(->tid, "m_grabInput")}
   __prop onground   {get: GetMembI(->tid, "m_onground")}
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
   int   tid;
   int   num;
   u64   ticks;
   __str name;
   int   pclass;
   __str pcstr;
   int   pronoun;
   int   dlgnum;
   int   fun;

   // Deltas
   anonymous
   struct player_delta cur;
   struct player_delta old;
   int oldhealth;
   int oldmana;

   // BIP
   struct bip bip, *bipptr;

   // Upgrades
   struct upgr_data upgrdata;
   upgrade_t        upgrades[UPGR_STATIC_MAX];
   upgrademap_t     upgrademap;

   uint upgrmax;
   bool upgrinit;

   // Inventory
   container_t inv[8];
   container_t misc;

   item_t *useitem;
   item_t *selitem;
   bool    movitem;
   bool    invinit;

   // HUD
   bool hudenabled;
   struct loginfo log;

   list_t hudstrlist;

   // Score
   i96     scoreaccum;
   __str   scoreaccumstr;
   int     scoreaccumtime;
   fixed64 scoremul;
   fixed64 discount;

   // Misc
   int decvars[8];

   fixed rage;
   char *notes[16];

   uint nextstep;
   bool hadinfrared;

   // Input
   char txtbuf[8];
   uint tbptr;

   // Static data
   uint  spawnhealth;
   fixed jumpheight;
   fixed viewheight;
   __str stepnoise;

   // pitch/yaw in precalculated sane radian format
   fixed64 pitchf;
   fixed64 yawf;

   // Additive angles
   fixed64 addpitch;
   fixed64 addyaw;
   fixed64 addroll;

   // Damage bob angles
   fixed64 bobpitch;
   fixed64 bobyaw;

   // Extra angles
   fixed64 extrpitch;
   fixed64 extryaw;

   // View TIDs
   int cameratid;
   int weathertid;

   // GUI
   uint  activegui;
   cbi_t cbi;

   // Statistics
   uint healthsum;
   uint healthused;

   i96 scoresum;
   i96 scoreused;

   uint unitstravelled;

   uint spuriousexplosions;
   uint brouzouf;

   uint weaponsheld;
   uint itemsbought;
   uint upgradesowned;

   // Weapons
   weapondata_t weapon;
   __str weaponclass;

   int  riflefiremode;
   bool autoreload;

   // Keys
   bool krc, kyc, kbc;
   bool krs, kys, kbs;

   // 🌌 「÷」 0
   bool sgacquired;
};

typedef void (*player_cb_t)(struct player *p);

// Extern Objects ------------------------------------------------------------|

#ifndef EXTERNAL_CODE
extern struct player players[MAX_PLAYERS];
#else
struct player (*Lith_GetPlayersExtern(void))[MAX_PLAYERS];
#define players (*Lith_GetPlayersExtern())
#endif

#endif
