#include "lith_upgrades_common.h"

#define UserData upgr->UserData_Adrenaline


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Update
//
[[__call("ScriptS")]]
void Upgr_Adrenaline_Update(player_t *p, upgrade_t *upgr)
{
   if(UserData.charge < 1050)
      UserData.charge++;
   
   else if(ACS_CheckInventory("Lith_AdrenalineToken"))
   {
      ACS_PlaySound(0, "player/adren/inj", 5|CHAN_NOPAUSE|CHAN_MAYBE_LOCAL|CHAN_UI, 1.0, false, ATTN_STATIC);
      Lith_LogH(p, ">>>>> Adrenaline administered.");
      
      UserData.charge = UserData.readied = 0;
      
      ACS_GiveInventory("Lith_TimeHax2", 1);
      ACS_Delay(36);
      ACS_TakeInventory("Lith_TimeHax2", 1);
   }
   
   else if(!UserData.readied)
   {
      ACS_PlaySound(0, "player/adren/ready", 5|CHAN_NOPAUSE|CHAN_MAYBE_LOCAL|CHAN_UI, 1.0, false, ATTN_STATIC);
      Lith_LogH(p, ">>>>> Adrenaline injector ready.");
      UserData.readied = 1;
   }
}

// EOF

