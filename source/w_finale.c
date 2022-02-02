/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Endings.
 *
 * ---------------------------------------------------------------------------|
 */

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

dynam_aut script
void F_Run() {
   pl.dlg.page = finale;
   pl.modal = _gui_dlg;
   Dlg_Run(DNUM_PRG_BEG);
}

script_str ext("ACS") addr(OBJ "Finale")
void Sc_Finale() {
   i32 boss = ServCallI(sm_GetBossLevel);
   i32 which;

   if(GetFun() & lfun_division) {
      which = _finale_division;
   } else {
      switch(boss) {
      case boss_none:        which = _finale_normal;      break;
      case boss_barons:      which = _finale_barons;      break;
      case boss_cyberdemon:  which = _finale_cyberdemon;  break;
      case boss_spiderdemon: which = _finale_spiderdemon; break;
      case boss_iconofsin:   which = _finale_icon_of_sin; break;
      case boss_other:       which = _finale_other;       break;
      }
   }

   F_Start(which);
}

/* EOF */
