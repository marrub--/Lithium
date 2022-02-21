// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Player data and functions.                                               │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#if defined(pclass_x)
#ifndef pclass_bit_x
#define pclass_bit_x(lng, eq)
#endif

/* Base Classes */
pclass_bit_x(pcl_marine_b,    0)
pclass_bit_x(pcl_cybermage_b, 1)
pclass_bit_x(pcl_informant_b, 2)
pclass_bit_x(pcl_wanderer_b,  3)
pclass_bit_x(pcl_assassin_b,  4)
pclass_bit_x(pcl_darklord_b,  5)
pclass_bit_x(pcl_thoth_b,     6)
pclass_bit_x(pcl_max_b,       7)

pclass_x(pM, pcl_marine,    1 << pcl_marine_b)
pclass_x(pC, pcl_cybermage, 1 << pcl_cybermage_b)
pclass_x(pI, pcl_informant, 1 << pcl_informant_b)
pclass_x(pW, pcl_wanderer,  1 << pcl_wanderer_b)
pclass_x(pA, pcl_assassin,  1 << pcl_assassin_b)
pclass_x(pD, pcl_darklord,  1 << pcl_darklord_b)
pclass_x(pT, pcl_thoth,     1 << pcl_thoth_b)

/* Groups */
pclass_x(gO, pcl_outcasts,   pcl_marine    | pcl_cybermage)
pclass_x(gM, pcl_missioners, pcl_informant | pcl_wanderer)
pclass_x(gI, pcl_intruders,  pcl_assassin  | pcl_darklord)
pclass_x(gF, pcl_finalizer,  pcl_thoth)

/* Lifeform Type */
pclass_x(gH, pcl_human,    pcl_marine   | pcl_cybermage | pcl_assassin)
pclass_x(gN, pcl_nonhuman, pcl_wanderer | pcl_darklord  | pcl_thoth)
pclass_x(gR, pcl_robot,    pcl_informant)

/* Misc. Abilities */
pclass_x(gA, pcl_any,       pcl_human     | pcl_nonhuman | pcl_robot)
pclass_x(gU, pcl_magicuser, pcl_cybermage | pcl_wanderer | pcl_thoth)
#undef pclass_x
#undef pclass_bit_x
#elif !defined(p_player_h)
#define p_player_h

enum ZscName(Attrib) {
   at_acc,
   at_def,
   at_str,
   at_vit,
   at_stm,
   at_luk,
   at_spc,
   at_max
};

enum ZscName(PData) {
   pdata_weapon,
   pdata_upgrade,
   pdata_riflemode,
   pdata_hassigil,
   pdata_weaponzoom,
   pdata_pclass,
   pdata_semifrozen,
   pdata_addp,
   pdata_addy,
   pdata_addr,
   pdata_recoilp,
   pdata_attr,
   pdata_alpha,
   pdata_oldhealth,
   pdata_hudenabled,
   pdata_flashbattery,
};

enum ZscName(SubweaponType) {
   _subw_gun,
   _subw_dagger,
   _subw_grenade,
   _subw_axe,
   _subw_dart,
   _subw_max,
};

enum ZscName(PClass) {
   pcl_unknown,

   #define pclass_bit_x(lng, eq) lng = eq,
   #define pclass_x(shr, lng, eq) lng = eq,
   #include "p_player.h"
};

#if !ZscOn
#include "common.h"
#include "p_cbi.h"
#include "p_upgrades.h"
#include "p_bip.h"
#include "p_log.h"
#include "m_list.h"
#include "p_weapons.h"
#include "p_shopdef.h"
#include "p_attrib.h"
#include "items.h"
#include "d_vm.h"

#include <GDCC/HashMap.h>

#define P_None() (!pl.active)
#define P_Wep_CurType() (pl.weapon.cur->info->type)

script void P_Init(void);
script void P_Data_Load(void);
script void P_Data_Save(void);
bool P_ButtonPressed(i32 bt);
optargs(1) bool P_SetVel(k32 velx, k32 vely, k32 velz, bool add);
void P_ValidateTID(void);
void P_GUI_Close(void);
void P_GUI_Use(void);
i96 P_Scr_Give(k32 x, k32 y, k32 z, i96 score, bool nomul);
i96 P_Scr_GivePos(i32 x, i32 y, i96 score, bool nomul);
void P_Scr_Take(i96 score);
void P_Lv_GiveEXP(u64 amt);
stkcall cstr P_Discrim(i32 pclass);
stkcall i32 P_Color(i32 pclass);
void P_Dat_PTickPst(void);
script extern void P_Scr_Payout(void);
void P_Log_SellWeapon(struct weaponinfo const *info, i96 score);
void P_Log_Weapon(struct weaponinfo const *info);

script void P_Wep_PTickPre (void);
       void P_Dat_PTickPre (void);
script void P_CBI_PTick    (void);
script void P_Inv_PTick    (void);
script void P_Log_PTick    (void);
script void P_Upg_PTick    (void);
script void P_Wep_PTick    (void);
script void P_Upg_PTickPst (void);
       void P_Ren_PTickPst (void);

script void P_TeleportIn (void);
sync   void P_TeleportOut(i32 tag);

#ifndef NDEBUG
void P_Ren_Debug(void);
#endif
void P_Ren_Magic(void);
script void P_Ren_Step(void);
void P_Ren_View(void);
script void P_Ren_Scope(void);

enum {
   #define pclass_x(shr, lng, eq) shr = lng,
   #include "p_player.h"
};

enum {
   _gui_none,
   _gui_cbi,
   _gui_waypoint,

   _gui_disables_hud,
   _gui_dlg = _gui_disables_hud,
   _gui_intro,
};

enum {
   _itm_disp_none,
   _itm_disp_log,
   _itm_disp_pop,
   _itm_disp_both,
   _itm_disp_max
};

enum {
   _hud_marine,
   _hud_cybermage,
   _hud_informant,
   _hud_wanderer,
   _hud_assassin,
   _hud_darklord,
   _hud_thoth,
   _hud_old,
   _hud_max
};

/* Data that needs to be kept track of between frames. */
struct player_delta {
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

   /* GUI */
   i32 modal;

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
struct player {
   /* data */
   __prop megaProtect  {default: PtrInvNum(->tid, so_MegaProtection)}
   __prop mana         {default: PtrInvNum(->tid, so_ManaAmmo)}
   __prop manamax      {default: ACS_GetMaxInventory(->tid, so_ManaAmmo)}
   __prop health       {default:   GetHealth(->tid),
                        operator=: SetHealth(->tid, ...)}
   __prop setActivator {operator(): ACS_SetActivator(->tid)}
   __prop getVel       {operator(): mag2k(->velx, ->vely)}
   __prop onground     {default: GetMembI(->tid, sm_OnGround)}
   __prop waterlevel   {default: GetMembI(->tid, sm_WaterLevel)}
   __prop classname    {default: GetNameTag(->tid)}
   __prop overdrive    {default: GetMembI(->tid, sm_Overdrive)}

   /* log */
   __prop logB {operator(): P_Log_Both()}
   __prop logF {operator(): P_Log_Full()}
   __prop logH {operator(): P_Log_HUDs()}

   /* Initialization */
   bool wasinit;
   bool active;
   bool dead;
   bool reinit;

   /* Info */
   i32  tid;
   i32  num;
   u64  ticks;
   str  name;
   i32  pclass;
   i32  pclass_b;
   str  pcstr;
   cstr discrim;
   i32  color;
   str  advice;
   cstr obit;

   /* Deltas */
   anonymous
   struct player_delta cur;
   struct player_delta old;
   i32 oldhealth;
   i32 oldmana;

   /* Upgrades */
   struct upgr_data upgrdata;
   struct upgrade   upgrades[UPGR_MAX];

   u32 autobuy;

   bool upgrinit;

   /* Inventory */
   struct container inv[_inv_num];

   struct item *useitem;
   struct item *selitem;
   struct item *opnitem;
   bool         movitem;
   bool         invinit;

   /* HUD */
   bool hudenabled;

   /* Score */
   i96 scoreaccum;
   str scoreaccumstr;
   i32 scoreaccumtime;
   k64 scoremul;

   /* Misc */
   k32 rage;

   char *notes[16];

   u32 nextstep;

   bool teleportedout;
   u32  done_intro;

   /* Input */
   struct gui_txt tb;

   /* Static data */
   u32 spawnhealth;
   k32 jumpheight;
   k32 viewheight;
   k32 attackheight;
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
   k64 bobroll;

   /* Extra angles */
   k64 extrpitch;
   k64 extryaw;
   k64 extrroll; /* throw em to the dogs -suwy */

   /* GUI */
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

   /* 🌌 「÷」 0 */
   bool sgacquired;
};
#endif

#if !ZscOn
extern struct player pl;
#endif

#endif
