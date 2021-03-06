/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * HeadsUpDisT upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

/* Extern Functions -------------------------------------------------------- */

void Upgr_HeadsUpDisT_Render(struct upgrade *upgr)
{
   Upgr_HeadsUpDisC_Render(upgr);

   k32 manaperc = pl.mana / (k32)pl.manamax;

   i32 hprc = ceilk(min(manaperc,        0.5k) * 2 * 62);
   i32 fprc = ceilk(max(manaperc - 0.5k, 0.0k) * 2 * 62);

   PrintSprite(sp_HUD_C_MagicIcon, 1,1, 213,2);
   PrintSprite(sp_HUD_C_BarSmall2, 1,1, 220,2);

   SetClip(2, 219-5, hprc, 5);
   PrintSprite(sp_HUD_C_ManaBar1, 2,1, 219,2);
   ClearClip();

   SetClip(2, 219-5, fprc, 5);
   PrintSprite(sp_HUD_C_ManaBar2, 2,1, 219,2);
   ClearClip();
}

/* EOF */
