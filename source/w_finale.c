// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Endings.                                                                 │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "common.h"
#include "p_player.h"
#include "w_world.h"

noinit static
i32 finale;

void F_Start(i32 which) {
   finale = which;
   ACS_ChangeLevel(sp_LITHEND, 0, CHANGELEVEL_NOINTERMISSION |
                   CHANGELEVEL_PRERAISEWEAPON, -1);
}

dynam_aut script void F_Run(void) {
   pl.setActivator();
   pl.dlg.page = finale;
   pl.modal    = _gui_dlg;
   Dlg_Run(DNUM_PRG_BEG);
}

script_str ext("ACS") addr(OBJ "Finale")
void Z_Finale(void) {
   i32 which;

   switch(ml.boss) {
   default:
      which = _finale_normal;
      break;
   case boss_iconofsin:
      if(GetFun() & lfun_division) {
         which = _finale_division;
      } else {
         which = _finale_icon_of_sin;
      }
      break;
   }

   F_Start(which);
}

/* EOF */
