/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Player data and functions.
 *
 * ---------------------------------------------------------------------------|
 */

#if defined(PCL)
/* Shorthand and classes. For parsing and headers. */
PCL(pM, pcl_marine)
PCL(pC, pcl_cybermage)
PCL(pI, pcl_informant)
PCL(pW, pcl_wanderer)
PCL(pA, pcl_assassin)
PCL(pD, pcl_darklord)
PCL(pT, pcl_thoth)

PCL(gO, pcl_outcasts)
PCL(gM, pcl_missioners)
PCL(gI, pcl_intruders)
PCL(gA, pcl_any)
PCL(gH, pcl_human)
PCL(gN, pcl_nonhuman)
PCL(gR, pcl_robot)
#undef PCL
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
#include "d_vm.h"

#include <GDCC/HashMap.h>

#define for_player() \
   for(i32 _piter = 0; _piter < _max_players; _piter++) \
      __with(struct player *p = &players[_piter];) \
         if(p->active)

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
bool P_ButtonPressed(struct player *p, i32 bt);
optargs(1) bool P_SetVel(struct player *p, k32 velx, k32 vely, k32 velz, bool add);
void P_ValidateTID(struct player *p);
void P_GUI_Close(struct player *p);
void P_GUI_Use(struct player *p);
optargs(1) i96 P_Scr_Give(struct player *p, i96 score, bool nomul);
void P_Scr_Take(struct player *p, i96 score);
script void P_GiveAllScore(i96 score, bool nomul);
script void P_GiveAllEXP(u64 amt);
void P_Lv_GiveEXP(struct player *p, u64 amt);
cstr P_Discrim(i32 pclass);
void P_Dat_PTickPst(struct player *p);
struct player *P_PtrFind(i32 tid, i32 ptr);

sync void P_TeleportIn(struct player *p);
sync void P_TeleportOut(struct player *p);
script void P_TeleportInAsync(struct player *p);
script void P_TeleportOutAsync(struct player *p);

/* Types ------------------------------------------------------------------- */

enum {
   _max_players = 8,
};

enum
{
   #define PCL(l, r) l = r,
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

/* Data that needs to be kept track of between frames. */
struct player_delta
{
   /* Status */
   k32 alpha;
   i96 score;
   u32 maxhealth;

   struct dlg_start_info dlg;

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

/*      07-04-2016: That's a lot of data!
 * edit 09-04-2016: Holy shit, that's really a lot of data!
 * edit 07-05-2016: JESUS TAKE THE WHEEL
 * edit 03-01-2017: help
 * edit 06-01-2017: there's so much data that I had to split it
 * edit 23-01-2017: D E S T R O Y
 * edit 26-02-2017: There is yet again so much data that I had to split it.
 * edit 11-03-2017: NOW WITH PROPERTY HELL
 * edit 11-07-2017: and now it's over 5000 bytes.
 * edit 14-07-2017: lol nevermind it's only 2kb now
 * edit 31-08-2017: m e r g e
 * edit 04-04-2020: WHY DID YOU MAKE IT THAT COMPLEX YOU BUNGUS
 */
struct player
{
   /* data */
   __prop megaProtect  {default: PtrInvNum(->tid, so_MegaProtection)}
   __prop mana         {default: PtrInvNum(->tid, so_Mana)}
   __prop manamax      {default: ACS_GetMaxInventory(->tid, so_Mana)}
   __prop health       {default:   GetMembI(->tid, sm_Health),
                        operator=: SetPropI(->tid, APROP_Health)}
   __prop setActivator {operator(): ACS_SetActivator(->tid)}
   __prop getVel       {operator(): mag2k(->velx, ->vely)}
   __prop grabInput    {default:   GetMembI(->tid, sm_GrabInput),
                        operator=: SetMembI(->tid, sm_GrabInput)}
   __prop onground     {default: GetMembI(->tid, sm_OnGround)}
   __prop waterlevel   {default: GetPropI(->tid, APROP_Waterlevel)}
   __prop classname    {default: GetPropS(->tid, APROP_NameTag)}
   __prop overdrive    {default: GetMembI(->tid, sm_Overdrive)}

   /* cvars */
   __prop getCVarI {operator(): ACS_GetUserCVar      (->num)}
   __prop getCVarK {operator(): ACS_GetUserCVarFixed (->num)}
   __prop getCVarS {operator(): ACS_GetUserCVarString(->num)}
   __prop setCVarI {operator(): ACS_SetUserCVar      (->num)}
   __prop setCVarK {operator(): ACS_SetUserCVarFixed (->num)}
   __prop setCVarS {operator(): ACS_SetUserCVarString(->num)}

   /* log */
   __prop logB {operator(): P_Log_Both(this)}
   __prop logF {operator(): P_Log_Full(this)}
   __prop logH {operator(): P_Log_HUDs(this)}

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
   cstr discrim;
   str advice;

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
   struct upgrade   upgrades[UPGR_MAX];

   u32 autobuy;

   bool upgrinit;

   /* Inventory */
   struct container inv[_inv_num];

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

   bool teleportedout;

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
   bool cbion;
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
   bool bladehit, rendhit;

   /* Keys */
   bool krc, kyc, kbc;
   bool krs, kys, kbs;

   /* 🌌 「÷」 0 */
   bool sgacquired;
};

/* Extern Objects ---------------------------------------------------------- */

extern struct player players[_max_players];

#endif
