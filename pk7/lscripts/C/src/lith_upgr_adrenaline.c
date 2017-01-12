#include "lith_upgrades_common.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Update
//
[[__call("ScriptS")]]
void Upgr_Adrenaline_Update(player_t *p, upgrade_t *upgr)
{
   if(upgr->user_int[0] < 1050)
      upgr->user_int[0]++;
   else if(ACS_CheckInventory("Lith_AdrenalineToken"))
   {
      ACS_PlaySound(0, "player/adren/inj", 5|CHAN_NOPAUSE|CHAN_MAYBE_LOCAL|CHAN_UI, 1.0, false, ATTN_STATIC);
      Lith_Log(p, ">>>>> Adrenaline administered.");
      
      upgr->user_int[0] = upgr->user_int[1] = 0;
      
      ACS_GiveInventory("Lith_TimeHax2", 1);
      ACS_Delay(35);
      ACS_TakeInventory("Lith_TimeHax2", 1);
   }
   else if(!upgr->user_int[1])
   {
      ACS_PlaySound(0, "player/adren/ready", 5|CHAN_NOPAUSE|CHAN_MAYBE_LOCAL|CHAN_UI, 1.0, false, ATTN_STATIC);
      Lith_Log(p, ">>>>> Adrenaline injector ready.");
      upgr->user_int[1] = 1;
   }
}

// EOF

