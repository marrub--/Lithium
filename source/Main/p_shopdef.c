// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "common.h"
#include "p_shopdef.h"
#include "p_player.h"
#include "w_world.h"

// Extern Functions ----------------------------------------------------------|

i96 Lith_ShopGetCost(struct player *p, struct shopdef const *def)
{
   return PlayerDiscount(def->cost);
}

bool Lith_ShopCanBuy(struct player *p, struct shopdef const *def, void *obj)
{
   return
      p->score - p->getCost(def) >= 0 &&
      (def->ShopCanBuy ? def->ShopCanBuy(p, def, obj) : true);
}

bool Lith_ShopBuy(struct player *p, struct shopdef const *def, void *obj, char const *namefmt, bool nodelivery, bool nolog)
{
   if(!p->canBuy(def, obj))
      return false;

   if(!nolog) p->logF("Bought %S", Language(namefmt, def->name)); // TODO

   if(def->bipunlock)
   {
      bip_name_t tag; lstrcpy_str(tag, def->bipunlock);
      p->bipUnlock(tag);
   }

   p->takeScore(p->getCost(def));

   bool delivered = false;

   if(!nodelivery && p->getCVarI(sc_player_teleshop))
   {
      i32 pufftid;
      i32 tid;

      ACS_LineAttack(0, p->yaw, p->pitch, 0, so_Dummy, so_NoDamage, 128.0, FHF_NORANDOMPUFFZ | FHF_NOIMPACTDECAL, pufftid = ACS_UniqueTID());

      k32 x = GetX(pufftid);
      k32 y = GetY(pufftid);
      k32 z = GetZ(pufftid);

      if((x || y || z) && ACS_Spawn(so_BoughtItem, x, y, z, tid = ACS_UniqueTID()))
      {
         if(def->ShopGive(p, def, obj, tid))
            p->logH(1, "\CjItem delivered."); // TODO
         else
            ACS_Thing_Remove(tid);

         delivered = true;
      }
      else
         p->logH(1, "\CgCouldn't deliver item\C-, placing directly in inventory."); // TODO
   }

   if(!delivered) def->ShopBuy(p, def, obj);

   return true;
}

// EOF
