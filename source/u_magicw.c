// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Magic upgrade.                                                           │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"
#include "w_world.h"

#define udata pl.upgrdata.magic

alloc_aut(0) script void GivePlayerZ(i32 tid);

Decl_Update void Upgr_MagicW_Update(void) {
   k32 manaperc = pl.mana / (k32)pl.manamax;
   if(udata.manaperc < 1 && manaperc == 1) {
      AmbientSound(ss_player_jem_manafull, 1.0);
   }
   udata.manaperc = manaperc;
   if(manaperc >= 0.7) {
      for(i32 i = 0; i < 5 * manaperc; i++) {
         k32 dst = ACS_RandomFixed(32, 56);
         k32 ang = ACS_RandomFixed(0, 1);
         i32 tid = ACS_UniqueTID();
         i32 x   = ACS_Cos(ang) * dst;
         i32 y   = ACS_Sin(ang) * dst;
         i32 z   = ACS_Random(8, 48);
         ACS_Spawn(so_ManaLeak, pl.x + x, pl.y + y, pl.z + z, tid);
         SetMembI(tid, sm_UserX, x);
         SetMembI(tid, sm_UserY, y);
         SetAlpha(tid, manaperc / 2);
         ACS_SetTranslation(tid, strp(_l(OBJ "Mana"), _p(ACS_Random(1, 8))));
         PtrSet(tid, AAPTR_DEFAULT, AAPTR_MASTER, pl.tid);
         GivePlayerZ(tid);
      }
   }
}

Decl_Render void Upgr_MagicW_Render(void) {
   if(!pl.hudenabled) return;
   i32 hprc = ceilk(mink(udata.manaperc,        0.5k) * 2 * 62);
   i32 fprc = ceilk(maxk(udata.manaperc - 0.5k, 0.0k) * 2 * 62);
   PrintSprite(sp_HUD_C_MagicIcon, pl.hudlpos+1,1, 213,2);
   PrintSprite(sp_HUD_C_BarSmall2, pl.hudlpos+1,1, 220,2);
   PrintSpriteClip(sp_HUD_C_ManaBar1, pl.hudlpos+2,1, 219,2, 0,0,hprc,5);
   PrintSpriteClip(sp_HUD_C_ManaBar2, pl.hudlpos+2,1, 219,2, 0,0,fprc,5);
}

/* EOF */
