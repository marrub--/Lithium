// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "lith_common.h"
#include "lith_shopdef.h"
#include "lith_player.h"
#include "lith_world.h"

StrEntON

// Extern Functions ----------------------------------------------------------|

i96 Lith_ShopGetCost(struct player *p, shopdef_t const *def)
{
   return PlayerDiscount(def->cost);
}

bool Lith_ShopCanBuy(struct player *p, shopdef_t const *def, void *obj)
{
   return
      p->score - p->getCost(def) >= 0 &&
      (def->shopCanBuy ? def->shopCanBuy(p, def, obj) : true);
}

bool Lith_ShopBuy(struct player *p, shopdef_t const *def, void *obj, __str namefmt, bool nodelivery, bool nolog)
{
   if(!p->canBuy(def, obj))
      return false;

   if(!nolog) p->logF("Bought %S", Language(namefmt, def->name));

   if(def->bipunlock)
   {
      bip_name_t tag; lstrcpy_str(tag, def->bipunlock);
      p->bipUnlock(tag);
   }

   p->takeScore(p->getCost(def));

   bool delivered = false;

   if(!nodelivery && p->getCVarI(sCVAR "player_teleshop"))
   {
      int pufftid;
      int tid;

      ACS_LineAttack(0, p->yaw, p->pitch, 0, OBJ "Dummy", OBJ "NoDamage", 128.0, FHF_NORANDOMPUFFZ | FHF_NOIMPACTDECAL, pufftid = ACS_UniqueTID());

      fixed x = GetX(pufftid);
      fixed y = GetY(pufftid);
      fixed z = GetZ(pufftid);

      if((x || y || z) && ACS_Spawn(OBJ "BoughtItem", x, y, z, tid = ACS_UniqueTID()))
      {
         if(def->shopGive(p, def, obj, tid))
            p->logH(1, "\CjItem delivered.");
         else
            ACS_Thing_Remove(tid);

         delivered = true;
      }
      else
         p->logH(1, "\CgCouldn't deliver item\C-, placing directly in inventory.");
   }

   if(!delivered) def->shopBuy(p, def, obj);

   return true;
}

// EOF

