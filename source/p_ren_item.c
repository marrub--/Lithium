/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_player.h"
#include "p_hudid.h"

void P_Ren_Infrared(struct player *p) {
   Str(power_lightamp, s"PowerLightAmp");

   bool hasir = InvNum(power_lightamp);

   if(!hasir && p->hadinfrared)
      ACS_LocalAmbientSound(ss_player_infraredoff, 127);

   p->hadinfrared = hasir;
}

/* EOF */
