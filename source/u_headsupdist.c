/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
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

stkcall
void Upgr_HeadsUpDisT_Activate(struct player *p, struct upgrade *upgr)
{
   Upgr_HeadsUpDisC_Activate(p, upgr);
}

stkcall
void Upgr_HeadsUpDisT_Deactivate(struct player *p, struct upgrade *upgr)
{
   Upgr_HeadsUpDisC_Deactivate(p, upgr);
}

stkcall
void Upgr_HeadsUpDisT_Render(struct player *p, struct upgrade *upgr)
{
   Upgr_HeadsUpDisC_Render(p, upgr);
}

/* EOF */
