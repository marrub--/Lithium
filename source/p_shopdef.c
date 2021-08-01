/* ---------------------------------------------------------------------------|
 *
 * Distributed under the CC0 public domain license.
 * By Alison G. Watson. Attribution is encouraged, though not required.
 * See licenses/cc0.txt for more information.
 *
 * ---------------------------------------------------------------------------|
 *
 * Unified shop definition handling.
 *
 * ---------------------------------------------------------------------------|
 */

#include "common.h"
#include "p_shopdef.h"
#include "p_player.h"
#include "w_world.h"

/* Extern Functions -------------------------------------------------------- */

i96 P_Shop_Cost(struct shopdef const *def)
{
   return P_Discount(def->cost);
}

bool P_Shop_CanBuy(struct shopdef const *def, void *obj)
{
   return pl.score - P_Shop_Cost(def) >= 0 &&
          (def->ShopCanBuy ? def->ShopCanBuy(def, obj) : true);
}

bool P_Shop_Buy(struct shopdef const *def, void *obj, cstr namefmt, bool nodelivery, bool nolog)
{
   if(!P_Shop_CanBuy(def, obj))
      return false;

   if(!nolog) {
      pl.logF(tmpstr(lang(sl_log_bought)), ns(lang_fmt(namefmt, def->name)));
   }

   if(def->bipunlock) {
      struct page *page = P_BIP_NameToPage(def->bipunlock);
      if(page) {
         P_BIP_Unlock(page, false);
      }
   }

   P_Scr_Take(P_Shop_Cost(def));

   bool delivered = false;

   if(!nodelivery && CVarGetI(sc_player_teleshop))
   {
      i32 tid;

      struct k32v3 v = trace_from(pl.yaw, pl.pitch, 128, pl.viewheight);

      if(ACS_Spawn(so_BoughtItem, v.x, v.y, v.z, tid = ACS_UniqueTID()))
      {
         if(def->ShopGive(def, obj, tid))
            pl.logH(1, tmpstr(lang(sl_log_delivered)));
         else
            ACS_Thing_Remove(tid);

         delivered = true;
      }
      else
         pl.logH(1, tmpstr(lang(sl_log_nodeliver)));
   }

   if(!delivered) def->ShopBuy(def, obj);

   return true;
}

/* EOF */
