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
   _pdt_soulsfreed,
   _pdt_ptid,
   _pdt_mflg,
   _pdt_menv,
   _pdt_rain,
   _pdt_mfunc,
   _pdt_mphantom,
   _pdt_msky,
};

/* External Data */
enum ZscName(EData) {
   _edt_bosslevel,
   _edt_paused,
   _edt_spawnhealth,
   _edt_viewheight,
   _edt_attackheight,
   _edt_lightning,
   _edt_heretic,
   _edt_chex,
   _edt_rampancy,
   _edt_legendoom,
   _edt_colorfulhell,
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
   lsc_worldopen,
   lsc_pdata,
   lsc_addangle,
   lsc_monsterinfo,
   lsc_raindropspawn,
   lsc_monstertype,
   lsc_drawcr,
   lsc_cheatinput,
   lsc_giveammo,
   lsc_hubclear,
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
   _mflg_corrupted,
   _mflg_thunder,
   _mflg_vacuum,
   _mflg_boss_beg,
   _mflg_boss_end = _mflg_boss_beg + 2,
   _mflg_env_beg,
   _mflg_env_end = _mflg_env_beg + 2,
   _mflg_rain_beg,
   _mflg_rain_end = _mflg_rain_beg + 3,
   _mflg_func_beg,
   _mflg_func_end = _mflg_func_beg + 2,
   _mflg_sky_beg,
   _mflg_sky_end = _mflg_sky_beg + 2,
};

enum ZscName(MapPhantom) {
   _mphantom_nospawn,
   _mphantom_none,
   _mphantom_spawned,
};

enum ZscName(MapRain) {
   _rain_none,
   _rain_rain,
   _rain_blood,
   _rain_abyss,
   _rain_snow,
   _rain_fire,
};

enum ZscName(MapEnvironment) {
   _menv_none,
   _menv_interstice,
   _menv_hell,
   _menv_abyss,
   _menv_evil,
};

enum ZscName(MapKind) {
   _mfunc_normal,
   _mfunc_title,
   _mfunc_end,
};

enum ZscName(MapSky) {
   _msky_nochange,
   _msky_vanilla,
   _msky_replace,
   _msky_shader,
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
   _skill_tourist   = 0,
   _skill_normal    = 2,
   _skill_hard      = 3,
   _skill_nightmare = 4,
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

enum compat_flag {
   _comp_legendoom,
   _comp_rampancy,
   _comp_drla,
   _comp_ch,
};

struct world {
   struct payoutinfo pay;

   i32  hubscleared;
   i32  secretsfound;
   i32  cbiperf;
   i64  cbiupgr;
   i32  compat;
   bool init;

   i32          a_x, a_y;
   struct polar a_angles[8];
   i32          a_cur;

   #ifndef NDEBUG
   str dbgstat[64];
   i32 dbgstatnum;
   #endif
};

struct map_locals {
   i32 mission;
   i32 temperature;
   i32 humidity;
   i32 soulsfreed;
   i32 boss;
   str name;
   str lump;
   i32 id;
   i32 seed;
   i32 flag;
   i32 prevscrts;
   i32 prevkills;
   i32 previtems;
   i32 missionkill;
   i32 missionprc;
   #ifndef NDEBUG
   str dbgnote[64];
   i32 dbgnotenum;
   #endif
};

extern struct world            wl;
extern struct map_locals lmvar ml;

void Scr_MInit(void);
script cstr CanonTime(i32 type, i32 time);
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
#endif

#endif
