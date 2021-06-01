/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * World data.
 *
 * ---------------------------------------------------------------------------|
 */

#if defined(w_setptr_x)
w_setptr_x(i32)
w_setptr_x(k32)
w_setptr_x(bool)
#undef w_setptr_x
#elif !defined(w_world_h)
#define w_world_h

#if !ZscOn
#include <stdbool.h>

#define InSecret (Cluster == 9 || Cluster == 10)
#define InHell   (Cluster >= 8)
#define OnEarth  (Cluster == 7)

#define MapNum  ACS_GetLevelInfo(LEVELINFO_LEVELNUM)
#define Cluster ACS_GetLevelInfo(LEVELINFO_CLUSTERNUM)

/* Line 1888300 is used as a control line for mod features.
 * Similarly, maps in a certain range are marked as Lithium maps.
 */
#define LithMapLine 0x001CD02C
#define LithMapBeg  0x001CCF00
#define LithMapEnd  0x001CD2E7

#define GetFun()  ACS_GetCVar(sc_fun)
#define SetFun(x) ACS_SetCVar(sc_fun, x)
#endif

/* Types ------------------------------------------------------------------- */

enum ZscName(WData) {
   wdata_bossspawned,
   wdata_soulsfreed,
   wdata_dorain,
   wdata_ptid,
   wdata_pclass,
};

enum ZscName(Fun) {
   lfun_ragnarok = 1 << 0,
   lfun_bips     = 1 << 1,
   lfun_final    = 1 << 2,
   lfun_division = 1 << 3,
};

enum ZscName(CBIUpgradeM) {
   cupg_weapninter,
   cupg_hasupgr1,
   cupg_armorinter,
   cupg_hasupgr2,
   cupg_weapninte2,
   cupg_rdistinter,
   cupg_max
};

enum ZscName(CBIUpgradeC) {
   cupg_c_slot3spell,
   cupg_c_slot4spell,
   cupg_c_slot5spell,
   cupg_c_slot6spell,
   cupg_c_slot7spell,
   cupg_c_rdistinter,
};

enum ZscName(RandomSpawnNum) {
   lrsn_garmor,
   lrsn_barmor,
   lrsn_hbonus,
   lrsn_abonus,
   lrsn_clip,
   lrsn_clipbx,
};

enum ZscName(MsgType) {
   msg_ammo,
   msg_huds,
   msg_full,
   msg_both
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
   lsc_getfontmetric,
   lsc_metr_xadv,
   lsc_metr_yofs,
   lsc_metr_tex,
   lsc_metr_w,
   lsc_metr_h,
   lsc_pdata,
   lsc_wdata,
   lsc_addangle,
   lsc_monsterinfo,
   lsc_raindropspawn,
   lsc_monstertype,
   lsc_drawdmgnum,
   lsc_preinit,
   lsc_worldreopen,
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

#if !ZscOn
enum {
   skill_tourist,
   skill_easy,
   skill_normal,
   skill_hard,
   skill_extrahard,
   skill_nightmare
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
   _unfinished,
   _finished,
   _failure
};

/* Extern Objects ---------------------------------------------------------- */

extern bool lmvar player_init;
extern struct payoutinfo payout;
extern bool singleplayer;
extern i32 mapscleared;
extern i32 prevcluster;
extern i32 mapseed;
extern bool unloaded;
extern bool lmvar islithmap;
extern i32 secretsfound;
extern k64 scoremul;
extern u64 ticks;
extern i32 soulsfreed;
extern bool bossspawned;
extern i32 cbiperf;
extern bool cbiupgr[cupg_max];
extern bool legendoom;
extern bool drlamonsters;
extern enum mission_status lmvar mission;
extern bool dorain;

/* DECORATE data */
extern i32 a_x, a_y;
extern struct polar a_angles[8];
extern i32 a_cur;

/* Extern Functions -------------------------------------------------------- */

void Scr_HInit(void);
cstr CanonTime(i32 type, u64 time);
optargs(1) i32 UniqueID(i32 tid);
void BeginAngles(i32 x, i32 y);
k32 AddAngle(i32 x, i32 y);
optargs(1) void FreezeTime(bool players_ok);
optargs(1) void UnfreezeTime(bool players_ok);
script void CBI_Install(i32 num);
script void W_Title(void);
script void F_Run(void);
void F_Load(void);
void F_Start(cstr which);
script void W_DoRain(void);
#endif

#endif
