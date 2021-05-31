/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
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

void Upgr_AutoReload_Activate(struct upgrade *upgr)
{
   pl.autoreload = true;
}

void Upgr_AutoReload_Deactivate(struct upgrade *upgr)
{
   pl.autoreload = false;
}

/* EOF */
