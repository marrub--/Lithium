#include "u_all.h"
#include "w_monster.h"

script_str ext("ACS") addr(OBJ "TriggerNegotiation")
void Z_TriggerNegotiation(void) {
   dmon_t *m = DmonSelf();
   if(!m) {
      return;
   }
   enum {
      negotiation_boss,
      negotiation_kid,
      negotiation_blackguard,
      negotiation_prince,
      negotiation_dude,
   };
   switch(m->mi->type) {
   case mtype_mastermind:
   case mtype_cyberdemon:
   case mtype_phantom:
      pl.dlg.page = negotiation_boss;
      break;
   case mtype_zombie:
   case mtype_lostsoul:
   case mtype_arachnotron:
      pl.dlg.page = negotiation_kid;
      break;
   case mtype_zombiesg:
   case mtype_imp:
   case mtype_hellknight:
   case mtype_revenant:
      pl.dlg.page = negotiation_blackguard;
      break;
   case mtype_baron:
   case mtype_painelemental:
   case mtype_archvile:
      pl.dlg.page = negotiation_prince;
      break;
   case mtype_zombiecg:
   case mtype_mancubus:
   case mtype_cacodemon:
   case mtype_demon:
      pl.dlg.page = negotiation_dude;
      break;
   case mtype_angel:
   case mtype_darkone:
   case mtype_person:
      return;
   }
   pl.dlg.num       = PNUM_RESERVED + 0;
   pl.dlg.firm_mode = FIRM_DIALOGUE;
}

/* EOF */
