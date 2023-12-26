#include "u_all.h"
#include "w_monster.h"

script_str ext("ACS") addr(OBJ "TriggerNegotiation")
void Z_TriggerNegotiation(void) {
   dmon_t *m = DmonSelf();
   if(!m) {
      return;
   }
   switch(m->mi->type) {
   case mtype_mastermind:
   case mtype_cyberdemon:
   case mtype_phantom:
      pl.dlg.page = 0;
      break;
   case mtype_zombie:
   case mtype_lostsoul:
      pl.dlg.page = 1;
      break;
   default:
   case mtype_zombiesg:
   case mtype_zombiecg:
   case mtype_imp:
   case mtype_demon:
   case mtype_mancubus:
   case mtype_arachnotron:
   case mtype_cacodemon:
   case mtype_hellknight:
   case mtype_baron:
   case mtype_revenant:
   case mtype_painelemental:
   case mtype_archvile:
   case mtype_angel:
   case mtype_darkone:
   case mtype_person:
      return;
   }
   pl.dlg.num       = PNUM_RESERVED + 0;
   pl.dlg.firm_mode = FIRM_DIALOGUE;
}

/* EOF */
