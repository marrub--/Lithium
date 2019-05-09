/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Player data and functions.
 *
 * ---------------------------------------------------------------------------|
 */

#if defined(LITH_X)
/* Shorthand and classes. For parsing and headers. */
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
#include "p_sys.h"
#include "items.h"

#include <GDCC/HashMap.h>

#define MAX_PLAYERS 8

#define for_player() \
   for(i32 _piter = 0; _piter < MAX_PLAYERS; _piter++) \
      __with(struct player *p = &players[_piter];) \
         if(p->active)

#define P_GiveAllScore(score, nomul) for_player() P_Scr_Give(p, score, nomul)
#define P_GiveAllEXP(amt) for_player() P_Lv_GiveEXP(p, amt)

#define LocalPlayer \
   (ACS_PlayerNumber() >= 0 ? &players[ACS_PlayerNumber()] : (struct player *)nil)
#define P_Discount(n) (i96)((n) * p->discount)
#define P_None(p) (!(p) || !(p)->active)
#define with_player(ptr) \
   __with(struct player *p = (ptr);) \
      if(!P_None(p))
#define P_Wep_CurType(p) ((p)->weapon.cur->info->type)

/* Extern Functions -------------------------------------------------------- */

script void P_Init(struct player *p);
script void P_Data_Load(struct player *p);
script void P_Data_Save(struct player *p);
stkcall bool P_ButtonPressed(struct player *p, i32 bt);
stkcall optargs(1) bool P_SetVel(struct player *p, k32 velx, k32 vely, k32 velz, bool add);
void P_ValidateTID(struct player *p);
stkcall void P_GUI_Close(struct player *p);
stkcall void P_GUI_Use(struct player *p, i32 type);
optargs(1) i96 P_Scr_Give(struct player *p, i96 score, bool nomul);
stkcall void P_Scr_Take(struct player *p, i96 score);
stkcall void P_Lv_GiveEXP(struct player *p, u64 amt);
stkcall struct upgrade *P_Upg_GetNamed(struct player *p, i32 name);
stkcall bool P_Upg_IsActive(struct player *p, i32 name);
stkcall cstr P_Discrim(i32 pclass);
stkcall void P_Dat_PTickPst(struct player *p);
struct player *P_PtrFind(i32 tid, i32 ptr);

/* Types ------------------------------------------------------------------- */

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

/* Data that needs to be kept track of between frames. */
struct player_delta
{
   /* Status */
   k32 alpha;
   i96 score;
   u32 maxhealth;
   u32 indialogue;

   /* Position */
   k32 x, y, z;
   k32 pitch, yaw, roll;

   /* Movement */
   k32 velx, vely, velz;
   k32 pitchv, yawv;
   k32 forwardv, sidev, upv;
   k32 speedmul;
   k32 jumpboost;

   /* Input */
   i32  buttons;
   bool scopetoken;
   u32  frozen;
   u32  semifrozen;

   /* Attributes */
   struct player_attributes attr;
};

/*       7/4/2016: That's a lot of data! */
/* edit  9/4/2016: Holy shit, that's really a lot of data! */
/* edit  7/5/2016: JESUS TAKE THE WHEEL */
/* edit  3/1/2017: help */
/* edit  6/1/2017: there's so much data that I had to split it */
/* edit 23/1/2017: D E S T R O Y */
/* edit 26/2/2017: There is yet again so much data that I had to split it. */
/* edit 11/3/2017: NOW WITH PROPERTY HELL */
/* edit 11/7/2017: and now it's over 5000 bytes. */
/* edit 14/7/2017: lol nevermind it's only 2kb now */
/* edit 31/8/2017: m e r g e */
struct player
{
   /* data */
   __prop mana          {get:  PtrInvNum(->tid, so_MagicAmmo)}
   __prop manamax       {get:  ACS_GetMaxInventory(->tid, so_MagicAmmo)}
   __prop health        {get: GetPropI(->tid, APROP_Health),
                         set: SetPropI(->tid, APROP_Health)}
   __prop setActivator  {call: ACS_SetActivator(->tid)}
   __prop getVel        {call: mag2k(->velx, ->vely)}
   __prop grabInput     {get: GetMembI(->tid, sm_GrabInput),
                         set: SetMembI(->tid, sm_GrabInput)}
   __prop onground      {get: GetMembI(->tid, sm_OnGround)}
   __prop waterlevel    {get: GetPropI(->tid, APROP_Waterlevel)}
   __prop classname     {get:  GetPropS(->tid, APROP_NameTag)}

   /* cvars */
   __prop getCVarI {call: ACS_GetUserCVar      (->num)}
   __prop getCVarK {call: ACS_GetUserCVarFixed (->num)}
   __prop getCVarS {call: ACS_GetUserCVarString(->num)}
   __prop setCVarI {call: ACS_SetUserCVar      (->num)}
   __prop setCVarK {call: ACS_SetUserCVarFixed (->num)}
   __prop setCVarS {call: ACS_SetUserCVarString(->num)}

   /* log */
   __prop logB {call: P_Log_Both(this)}
   __prop logF {call: P_Log_Full(this)}
   __prop logH {call: P_Log_HUDs(this)}

   /* upgrades */
   __prop getUpgr       {call: P_Upg_GetNamed(this)}
   __prop getUpgrActive {call: P_Upg_IsActive(this)}

   /* Initialization */
   bool wasinit;
   bool active;
   bool dead;
   bool reinit;

   /* Info */
   i32 tid;
   i32 num;
   u64 ticks;
   str name;
   i32 pclass;
   str pcstr;
   i32 pronoun;
   i32 dlgnum;
   i32 fun;
   cstr discrim;

   /* Deltas */
   anonymous
   struct player_delta cur;
   struct player_delta old;
   i32 oldhealth;
   i32 oldmana;

   /* BIP */
   struct bip bip;

   /* Upgrades */
   struct upgr_data upgrdata;
   struct upgrade   upgrades[UPGR_STATIC_MAX];
   upgrademap_t     upgrademap;

   u32  upgrmax;
   bool upgrinit;

   /* Inventory */
   struct container inv[8];
   struct container misc;

   struct item *useitem;
   struct item *selitem;
   bool         movitem;
   bool         invinit;

   /* HUD */
   bool hudenabled;

   list hudstrlist;

   struct loginfo log;

   /* Score */
   i96 scoreaccum;
   str scoreaccumstr;
   i32 scoreaccumtime;
   k64 scoremul;
   k64 discount;

   /* Misc */
   k32 rage;

   char *notes[16];

   u32  nextstep;
   bool hadinfrared;

   /* Input */
   char txtbuf[8];
   u32  tbptr;

   /* Static data */
   u32 spawnhealth;
   k32 jumpheight;
   k32 viewheight;
   str stepnoise;

   /* pitch/yaw in precalculated sane radian format */
   k64 pitchf;
   k64 yawf;

   /* Additive angles */
   k64 addpitch;
   k64 addyaw;
   k64 addroll;

   /* Damage bob angles */
   k64 bobpitch;
   k64 bobyaw;

   /* Extra angles */
   k64 extrpitch;
   k64 extryaw;

   /* View TIDs */
   i32 cameratid;
   i32 weathertid;

   /* GUI */
   u32 activegui;
   struct cbi cbi;

   /* Statistics */
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

   /* Weapons */
   struct weapondata weapon;
   str weaponclass;

   i32  riflefiremode;
   bool autoreload;

   /* Keys */
   bool krc, kyc, kbc;
   bool krs, kys, kbs;

   /* 🌌 「÷」 0 */
   bool sgacquired;
};

/* Extern Objects ---------------------------------------------------------- */

extern struct player players[MAX_PLAYERS];

#endif
