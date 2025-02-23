// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Endings.                                                                 │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_player.h"
#include "w_world.h"

noinit static i32 finale_override;

void F_Start(i32 which) {
   Dbg_Log(log_dev, _l("forcing finale "), _p(which));
   finale_override = which + 1;
   ACS_ChangeLevel(sp_LITHEND, 0, CHANGELEVEL_NOINTERMISSION |
                   CHANGELEVEL_PRERAISEWEAPON, -1);
}

dynam_aut script void F_Run(void) {
   Dbg_Log(log_dev, _l("beginning finale dialogue"));
   i32 which;
   if(finale_override) {
      which = finale_override - 1;
   } else if(oldml.boss == boss_iconofsin && GetFun() & lfun_division) {
      which = _finale_division;
   } else {
      which = _finale_normal;
   }
   pl.setActivator();
   pl.hudenabled = false;
   ACS_SetPlayerProperty(true, true, PROP_TOTALLYFROZEN);
   pl.dlg.num       = 0;
   pl.dlg.page      = which;
   pl.dlg.firm_mode = FIRM_FINALE;
   Dlg_Run();
}

/* EOF */
