#include "lith_common.h"
#include "lith_shopdef.h"
#include "lith_player.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Lith_ShopGetCost
//
score_t Lith_ShopGetCost(struct player_s *p, shopdef_t const *def)
{
   return PlayerDiscount(def->cost);
}

//
// Lith_ShopCanBuy
//
bool Lith_ShopCanBuy(player_t *p, shopdef_t const *def, void *obj)
{
   return
      p->score - Lith_ShopGetCost(p, def) >= 0 &&
      def->shopCanBuy(p, def, obj);
}

//
// Lith_ShopBuy
//
bool Lith_ShopBuy(player_t *p, shopdef_t const *def, void *obj, __str namefmt)
{
   if(!Lith_ShopCanBuy(p, def, obj))
      return false;
   
   Lith_LogF(p, "> Bought %LS", StrParam(namefmt, def->name));
   
   if(def->bipunlock)
      Lith_UnlockBIPPage(&p->bip, def->bipunlock);
   
   Lith_TakeScore(p, Lith_ShopGetCost(p, def));
   
   def->shopBuy(p, def, obj);
   
   return true;
}

// EOF

