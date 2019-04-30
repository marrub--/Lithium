/* pk7/lzscript/Constants/w_scorenums.zsc
 * ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Score numbers for various items and enemies.
 *
 * ---------------------------------------------------------------------------|
 */

enum /* ScoreNum */
{
   /* Score values */
   Score_Clip      = 1000,
   Score_ClipBox   = 4000,
   Score_Shell     = 2000,
   Score_ShellBox  = 5000,
   Score_Rocket    = 3000,
   Score_RocketBox = 6000,
   Score_Cell      = 4000,
   Score_CellPack  = 7000,
   Score_Backpack  = 10000,

   /* Tier 1 */
   Score_ZombieMan   = 1000, /* Bullets */
   Score_ShotgunGuy  = 2000, /* Bullets */
   Score_ChaingunGuy = 2000, /* Bullets */
   Score_Imp         = 2000, /* Fire */
   Score_DRLACaptain = 2000, /* Bullets */

   /* Tier 2 */
   Score_Demon      = 5000, /* Melee */
   Score_Spectre    = 7500, /* Melee */
   Score_LostSoul   = 500,  /* Melee */
   Score_Nitrogolem = 4000, /* Melee, FireMagic */

   /* Tier 3 */
   Score_HellKnight  = 7000,              /* Melee, Magic */
   Score_Revenant    = 7000,              /* Melee, Shrapnel */
   Score_Cacodemon   = 7000,              /* Melee, Energy */
   Score_Arachnotron = 7000,              /* Energy */
   Score_Mancubus    = 7000,              /* Fire */
   Score_BaronOfHell = 8000,              /* Melee, Magic */
   Score_Ophidian    = Score_Arachnotron, /* Ice, Fire */

   /* Tier 4 */
   Score_PainElemental = 20000, /* None */
   Score_Archvile      = 40000, /* FireMagic */

   /* Tier 5 */
   Score_SpiderDemon = 700000,            /* Bullets */
   Score_CyberDemon  = 1500000,           /* Shrapnel */
   Score_Maulotaur   = Score_SpiderDemon, /* Melee, Fire */

   /* Tier 6 */
   Score_DSparil = 10000000, /* Energy */
};

enum /* EXPNum */
{
   /* Tier 1 */
   Exp_ZombieMan   = 5,
   Exp_ShotgunGuy  = 10,
   Exp_ChaingunGuy = 15,
   Exp_Imp         = 5,
   Exp_DRLACaptain = 30,

   /* Tier 2 */
   Exp_Demon      = 10,
   Exp_Spectre    = Exp_Demon,
   Exp_LostSoul   = 5,
   Exp_Nitrogolem = 20,

   /* Tier 3 */
   Exp_HellKnight  = 80,
   Exp_Revenant    = 80,
   Exp_Cacodemon   = 50,
   Exp_Arachnotron = 80,
   Exp_Mancubus    = 50,
   Exp_BaronOfHell = 100,
   Exp_Ophidian    = 50,

   /* Tier 4 */
   Exp_PainElemental = 100,
   Exp_Archvile      = 200,

   /* Tier 5 */
   Exp_SpiderDemon = 3000,
   Exp_CyberDemon  = 4000,
   Exp_Maulotaur   = 1000,

   /* Tier 6 */
   Exp_DSparil = 9000,
};

/* EOF */
