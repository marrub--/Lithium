/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison Sanderson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * AutoReload upgrade.
 *
 * ---------------------------------------------------------------------------|
 */

#include "u_common.h"

/* Extern Functions -------------------------------------------------------- */

stkcall
void Upgr_AutoReload_Activate(struct player *p, struct upgrade *upgr)
{
   p->autoreload = true;
}

stkcall
void Upgr_AutoReload_Deactivate(struct player *p, struct upgrade *upgr)
{
   p->autoreload = false;
}

/* EOF */
