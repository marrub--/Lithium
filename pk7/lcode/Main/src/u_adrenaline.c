#include "lith_upgrades_common.h"

#define UserData upgr->UserData_Adrenaline


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Activate
//
void Upgr_Adrenaline_Activate(player_t *p, upgrade_t *upgr)
{
   ACS_TakeInventory("Lith_AdrenalineToken", 1);
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_Adrenaline_Update(player_t *p, upgrade_t *upgr)
{
   // Charge
   if(UserData.charge < 30 * 35)
      UserData.charge++;
   
   // Prepare
   else if(!UserData.readied)
   {
      ACS_PlaySound(0, "player/adren/ready", 5|CHAN_NOPAUSE|CHAN_MAYBE_LOCAL|CHAN_UI, 1.0, false, ATTN_STATIC);
      Lith_LogH(p, ">>>>> Adrenaline injector ready.");
      UserData.readied = true;
   }
   
   // Ready
   else if(!ACS_CheckInventory("Lith_AdrenalineToken"))
      ACS_GiveInventory("Lith_AdrenalineProjectileChecker", 1);
   
   // Use
   else
   {
      ACS_TakeInventory("Lith_AdrenalineToken", 1);
      
      ACS_PlaySound(0, "player/adren/inj", 5|CHAN_NOPAUSE|CHAN_MAYBE_LOCAL|CHAN_UI, 1.0, false, ATTN_STATIC);
      Lith_LogH(p, ">>>>> Adrenaline administered.");
      
      UserData.charge = UserData.readied = 0;
      
      ACS_GiveInventory("Lith_TimeHax2", 1);
      ACS_Delay(36);
      ACS_TakeInventory("Lith_TimeHax2", 1);
   }
}

// EOF

