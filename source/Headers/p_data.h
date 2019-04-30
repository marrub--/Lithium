/* pk7/lzscript/Constants/p_data.zsc
 * pk7_dt/dtzsc/pdata.zsc
 * ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Exposed player data.
 *
 * ---------------------------------------------------------------------------|
 */

enum /* LAttrib */
{
   at_acc,
   at_def,
   at_str,
   at_vit,
   at_stm,
   at_luk,
   at_spc,
   at_max
};

enum /* PData */
{
   pdata_weapon,
   pdata_upgrade,
   pdata_riflemode,
   pdata_hassigil,
   pdata_weaponzoom,
   pdata_pclass,
   pdata_semifrozen,
   pdata_slot3ammo,
   pdata_addp,
   pdata_addy,
   pdata_recoilp,
   pdata_attr,
};

enum /* PClass */
{
   pcl_unknown,

   /* Base Classes */
   pcl_marine    = 1 << 0,
   pcl_cybermage = 1 << 1,
   pcl_informant = 1 << 2,
   pcl_wanderer  = 1 << 3,
   pcl_assassin  = 1 << 4,
   pcl_darklord  = 1 << 5,
   pcl_thoth     = 1 << 6,
   pcl_doubletap = 1 << 7,

   /* Groups */
   pcl_outcasts   = pcl_marine    | pcl_cybermage,
   pcl_missioners = pcl_informant | pcl_wanderer,
   pcl_intruders  = pcl_assassin  | pcl_darklord | pcl_thoth,

   /* Lifeform Type */
   pcl_human    = pcl_marine    | pcl_cybermage | pcl_assassin,
   pcl_nonhuman = pcl_wanderer  | pcl_darklord  | pcl_thoth,
   pcl_robot    = pcl_informant | pcl_doubletap,

   /* Misc. Abilities */
   pcl_any       = pcl_human | pcl_nonhuman | pcl_robot,
   pcl_magicuser = pcl_cybermage | pcl_wanderer | pcl_thoth,
};

/* EOF */
