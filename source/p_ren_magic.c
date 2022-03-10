// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "p_hudid.h"

void P_Ren_Magic(void) {
   SetSize(800, 600);

   for(i32 i = 0; i < 4; i++) {
      i32 fid = fid_rendS + i;
      if(CheckFade(fid)) {
         PrintSpriteFP(pl.rendhit ? sa_rend_ht[i] : sa_rend_nh[i],
                       400,0, 300,0, fid);
      }
   }

   if(CheckFade(fid_blade)) {
      PrintSpriteFP(pl.bladehit ? sp_Weapon_BladeHit : sp_Weapon_Blade,
                    400,0, 300,0, fid_blade);
   }
}

/* EOF */
