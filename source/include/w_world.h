// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ World data.                                                              │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#if defined(w_setptr_x)
w_setptr_x(i32)
w_setptr_x(k32)
w_setptr_x(bool)
#undef w_setptr_x
#elif !defined(w_world_h)
#define w_world_h

#if !ZscOn
#include <stdbool.h>

#define MapNum  ACS_GetLevelInfo(LEVELINFO_LEVELNUM)
#define Cluster ACS_GetLevelInfo(LEVELINFO_CLUSTERNUM)

/* Line 1888300 is used as a control line for mod features.
 * Similarly, maps in a certain range are marked as Lithium maps.
 */
enum {
   LithMapLine = 0x001CD02C,

   LithMapBeg = 0x001CCF00,
   LithMapTest = LithMapBeg,
   LithMapAbyss,
   LithMapM1A1 = LithMapBeg + 100,
   LithMapEnd = 0x001D2BE1,
};

#define GetFun()  CVarGetI(sc_fun)
#define SetFun(x) CVarSetI(sc_fun, x)

#define EDataI(...) ServCallI(sm_EDataI, __VA_ARGS__)
#define EDataS(...) ServCallS(sm_EDataS, __VA_ARGS__)
#endif

/* Program Data */
enum ZscName(PData) {
   _pdt_weapon,
   _pdt_upgrade,
   _pdt_riflemode,
   _pdt_hassigil,
   _pdt_weaponzoom,
   _pdt_pclass,
   _pdt_semifrozen,
   _pdt_addp,
   _pdt_addy,
   _pdt_addr,
   _pdt_recoilp,
   _pdt_attr,
   _pdt_alpha,
   _pdt_oldhealth,
   _pdt_hudenabled,
   _pdt_flashbattery,
   _pdt_bossspawned,
   _pdt_soulsfreed,
   _pdt_ptid,
   _pdt_mapf,
   _pdt_mapc,
   _pdt_mapr,
   _pdt_mapk,
};

/* External Data */
enum ZscName(EData) {
   _edt_bosslevel,
   _edt_paused,
   _edt_spawnhealth,
   _edt_heretic,
   _edt_chex,
   _edt_rampancy,
   _edt_riflegrenade,

   _edt_obituary,
   _edt_logname,
   _edt_bipname,
   _edt_sky1,
};

enum ZscName(Fun) {
   lfun_ragnarok = 1 << 0,
   lfun_bips     = 1 << 1,
   lfun_division = 1 << 2,
   lfun_tainted  = 1 << 3,
   lfun_lane     = 1 << 4,
};

enum ZscName(BossRewardNum) {
   bossreward_easy_1,
   bossreward_easy_2,
   bossreward_med_1,
   bossreward_med_2,
   bossreward_med_3,
   bossreward_hard_1,
   bossreward_max,
};

enum ZscName(RandomSpawnNum) {
   lrsn_garmor,
   lrsn_barmor,
   lrsn_hbonus,
   lrsn_abonus,
};

enum ZscName(MsgType) {
   msg_huds,
   msg_full,
   msg_item,
   msg_scri,
};

enum ZscName(DamageType) {
   ldt_bullets,
   ldt_energy,
   ldt_fire,
   ldt_firemagic,
   ldt_magic,
   ldt_melee,
   ldt_shrapnel,
   ldt_ice,
   ldt_hazard,
   ldt_none
};

enum ZscName(ScriptNum) {
   lsc_gsinit = 17000,
   lsc_strinit,
   lsc_pdata,
   lsc_addangle,
   lsc_monsterinfo,
   lsc_raindropspawn,
   lsc_monstertype,
   lsc_worldreopen,
   lsc_worldopen,
   lsc_playeropen,
   lsc_drawcr,
   lsc_cheatinput,
   lsc_versionname,
   #define w_setptr_x(x) lsc_setptr##x,
   #include "w_world.h"
};

enum ZscName(BossType) {
   boss_none,
   boss_barons,
   boss_cyberdemon,
   boss_spiderdemon,
   boss_iconofsin,
   boss_other,
};

enum ZscName(WeaponName) {
   wepnam_fist,
   wepnam_chainsaw,
   wepnam_pistol,
   wepnam_shotgun,
   wepnam_supershotgun,
   wepnam_chaingun,
   wepnam_rocketlauncher,
   wepnam_plasmarifle,
   wepnam_bfg9000,
   wepnam_max,
};

enum ZscName(RifleMode) {
   rifle_firemode_auto,
   rifle_firemode_grenade,
   rifle_firemode_burst,
   rifle_firemode_max
};

enum ZscName(MapFlag) {
   /* states */
   _mapf_corrupted,
   _mapf_thunder,
   _mapf_vacuum,

   /* visual info */
   _mapf_skyreplace,

   /* categories */
   _mapf_cat_beg,
   _mapf_cat_end = _mapf_cat_beg + 2,
   _mapf_rain_beg,
   _mapf_rain_end = _mapf_rain_beg + 3,
   _mapf_kind_beg,
   _mapf_kind_end = _mapf_kind_beg + 2,
};

enum ZscName(MapRain) {
   _mapr_none,
   _mapr_rain,
   _mapr_blood,
   _mapr_abyss,
   _mapr_snow,
};

enum ZscName(MapCategory) {
   _mapc_none,
   _mapc_lithium,
   _mapc_interstice,
   _mapc_hell,
};

enum ZscName(MapKind) {
   _mapk_normal,
   _mapk_title,
   _mapk_end,
};

#if !ZscOn
enum {
   cupg_rdistinter,
   cupg_m_weapninter,
   cupg_m_cpu1,
   cupg_m_armorinter,
   cupg_m_cpu2,
   cupg_m_weapninte2,
   cupg_c_slot3spell,
   cupg_c_slot4spell,
   cupg_c_slot5spell,
   cupg_c_slot6spell,
   cupg_c_slot7spell,
   cupg_d_motra,
   cupg_d_zaruk,
   cupg_d_zakwu,
   cupg_d_zikr,
   cupg_d_shield,
   cupg_d_dimdriver,
};

enum {
   _skill_normal,
   _skill_hard,
   _skill_tourist,
   _skill_nightmare
};

enum {
   ct_full,
   ct_short,
   ct_date
};

struct payoutinfo {
   i32 killnum, killmax, killscr;
   i32 itemnum, itemmax, itemscr;
   i32 scrtnum, scrtmax, scrtscr;

   k64 killpct, itempct, scrtpct;

   i32 total;
   i32 tax;

   i32 par;

   struct {
      i32 kill100;
      i32 item100;
      i32 scrt100;
      i32 par;
      i32 sponsor;
   } activities;
};

enum mission_status {
   _mstat_unfinished,
   _mstat_finished,
   _mstat_failure
};

enum finale_num {
   _finale_normal,
   _finale_icon_of_sin,
   _finale_time_out,
   _finale_division,
};

struct world {
   struct payoutinfo pay;
   i32               mapscleared;
   bool              unloaded;
   i32               secretsfound;
   i32               ticks;
   bool              bossspawned;
   i32               cbiperf;
   i64               cbiupgr;
   bool              legendoom;
   bool              init;

   i32          a_x, a_y;
   struct polar a_angles[8];
   i32          a_cur;
};

struct map_locals {
   i32  mission;
   i32  temperature;
   i32  humidity;
   i32  soulsfreed;
   i32  boss;
   str  name;
   str  lump;
   i32  id;
   i32  seed;
   i32  flag;
   bool init;
};

extern struct world            wl;
extern struct map_locals lmvar ml;

void Scr_HInit(void);
cstr CanonTime(i32 type, i32 time);
optargs(1) i32 UniqueID(i32 tid);
void BeginAngles(i32 x, i32 y);
k32 AddAngle(i32 x, i32 y);
optargs(1) stkcall void FreezeTime(bool players_ok);
optargs(1) stkcall void UnfreezeTime(bool players_ok);
script void CBI_Install(i32 num);
script void W_Title(void);
script void F_Run(void);
void F_Load(void);
void F_Start(i32 which);
script void W_DoRain(void);
#endif

#endif
