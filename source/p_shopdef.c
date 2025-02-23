// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Marrub / Alison.  Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Unified shop definition handling.                                        │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "m_engine.h"
#include "p_shopdef.h"
#include "p_player.h"
#include "w_world.h"

bool P_Shop_CanBuy(struct shopdef *def) {
   if(pl.score - def->cost >= 0) {
      switch(def->shoptype) {
      case _shop_items:    return Shop_CanBuy((struct shopitem const *)def);
      case _shop_upgrades: return Upgr_CanBuy((struct upgrade  const *)def);
      }
   }
   return false;
}

bool P_Shop_Give(struct shopdef *def, i32 tid) {
   switch(def->shoptype) {
   case _shop_items:    return Shop_Give((struct shopitem const *)def, tid);
   case _shop_upgrades: return Upgr_Give((struct upgrade  const *)def, tid);
   }
   return false;
}

bool P_Shop_Buy(struct shopdef *def, cstr name, bool nodelivery, bool nolog) {
   if(!P_Shop_CanBuy(def)) {
      return false;
   }

   if(!nolog) {
      P_LogF(tmpstr(sl_log_bought),
              ns(lang_discrim(fast_strdup2(name, def->name))));
   }

   if(def->bipunlock) {
      P_BIP_UnlockName(def->bipunlock, true);
   }

   P_Scr_Take(def->cost);

   bool delivered = false;

   if(!nodelivery && CVarGetI(sc_player_teleshop)) {
      i32 tid;

      struct k32v3 v = trace_from(pl.yaw, pl.pitch, 128, pl.viewheight);

      if(ACS_Spawn(so_BoughtItem, v.x, v.y, v.z, tid = ACS_UniqueTID())) {
         if(P_Shop_Give(def, tid)) {
            P_LogH(1, tmpstr(sl_log_delivered));
         } else {
            ACS_Thing_Remove(tid);
         }

         delivered = true;
      } else {
         P_LogH(1, tmpstr(sl_log_nodeliver));
      }
   }

   if(!delivered) {
      switch(def->shoptype) {
      case _shop_items:
         Shop_Buy((struct shopitem const *)def);
         break;
      case _shop_upgrades:
         P_Upg_SetOwned((struct upgrade *)def);
         break;
      }
   }

   return true;
}

/* EOF */
