/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Exposed world data.
 *
 * ---------------------------------------------------------------------------|
 */
/* decompat-out pk7/lzscript/Constants/w_data.zsc */

enum /* WData */
{
   wdata_bossspawned,
   wdata_soulsfreed,
   wdata_dorain,
   wdata_ptid,
   wdata_pclass,
};

enum /* Fun */
{
   lfun_ragnarok = 1 << 0,
   lfun_bips     = 1 << 1,
   lfun_final    = 1 << 2,
   lfun_division = 1 << 3,
};

enum /* CBIUpgradeM */
{
   cupg_weapninter,
   cupg_hasupgr1,
   cupg_armorinter,
   cupg_hasupgr2,
   cupg_weapninte2,
   cupg_rdistinter,
   cupg_max
};

enum /* CBIUpgradeC */
{
   cupg_c_slot3spell,
   cupg_c_slot4spell,
   cupg_c_slot5spell,
   cupg_c_slot6spell,
   cupg_c_slot7spell,
   cupg_c_rdistinter,
};

enum /* RandomSpawnNum */
{
   lrsn_garmor,
   lrsn_barmor,
   lrsn_hbonus,
   lrsn_abonus,
   lrsn_clip,
   lrsn_clipbx,
};

enum /* MsgType */
{
   msg_ammo,
   msg_huds,
   msg_full,
   msg_both
};

enum /* DamageType */
{
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

enum /* ScriptNum */
{
   lsc_getfontmetric = 17100,
   lsc_metr_xadv,
   lsc_metr_yofs,
   lsc_metr_tex,
   lsc_metr_w,
   lsc_metr_h,
   lsc_drawplayericon,
   lsc_pdata,
   lsc_wdata,
   lsc_addangle,
   lsc_monsterinfo,
   lsc_raindropspawn,
   lsc_monstertype,
   lsc_drawdmgnum,
};

enum /* BossType */
{
   boss_none,
   boss_barons,
   boss_cyberdemon,
   boss_spiderdemon,
   boss_iconofsin,
   boss_other,
};

/* EOF */
