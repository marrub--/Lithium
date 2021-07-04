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

void P_Ren_Magic()
{
   SetSize(800, 600);

   for(i32 i = 0; i < 4; i++) {
      StrAry(na, s":Weapon:Rend1", s":Weapon:Rend2", s":Weapon:Rend3",
                 s":Weapon:Rend4");
      StrAry(ha, s":Weapon:RendHit1", s":Weapon:RendHit2", s":Weapon:RendHit3",
                 s":Weapon:RendHit4");

      i32 fid = fid_rendS + i;
      if(CheckFade(fid))
         PrintSpriteFP(pl.rendhit ? ha[i] : na[i], 0,1, 0,1, fid);
   }

   if(CheckFade(fid_blade)) {
      PrintSpriteFP(pl.bladehit ? sp_Weapon_BladeHit : sp_Weapon_Blade,
                    0,1, 0,1, fid_blade);
   }
}

/* EOF */
