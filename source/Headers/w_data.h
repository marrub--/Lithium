/* pk7/lzscript/Constants/w_data.zsc
 * pk7_dt/dtzsc/wdata.zsc
 */
/*
 * ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Exposed world data.
 *
 * ---------------------------------------------------------------------------|
 */

enum /* WData */
{
   wdata_bossspawned,
   wdata_enemycheck,
   wdata_fun,
   wdata_soulsfreed,
   wdata_dorain,
   wdata_ptid,
   wdata_pclass,
};

enum /* Fun */
{
   lfun_ragnarok = 1 << 0,
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

enum /* ArmorSlot */
{
   aslot_lower,
   aslot_upper,
   aslot_ring,
   aslot_pauld,
   aslot_max
};

enum /* MsgType */
{
   msg_ammo,
   msg_huds,
   msg_full,
   msg_both
};

enum /* LDamageType */
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

enum /* LScriptNum */
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
};

enum /* Lith_Channel */
{
   lch_auto,

   lch_weapon,
   lch_voice,
   lch_item,
   lch_body,

   lch_altweapon,
   lch_body2,
   lch_body3,
   lch_dryweapon,
   lch_idleweapon,
   lch_item2,
   lch_reload,
   lch_reload2,
   lch_reload3,
   lch_scope,
   lch_weapon2,
   lch_weapon3,
};

/* EOF */
