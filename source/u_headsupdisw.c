/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * HeadsUpDisW upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

/* Extern Functions -------------------------------------------------------- */

stkcall
void Upgr_HeadsUpDisW_Activate(struct player *p, struct upgrade *upgr)
{
   Upgr_HeadsUpDisM_Activate(p, upgr);
}

stkcall
void Upgr_HeadsUpDisW_Deactivate(struct player *p, struct upgrade *upgr)
{
   Upgr_HeadsUpDisM_Deactivate(p, upgr);
}

stkcall
void Upgr_HeadsUpDisW_Render(struct player *p, struct upgrade *upgr)
{
   Upgr_HeadsUpDisM_Render(p, upgr);
}

/* EOF */
