#include "lith_upgrades_common.h"


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Deactivate
//
void Upgr_RifleModes_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->riflefiremode = 0;
}

//
// Render
//
void Upgr_RifleModes_Render(player_t *p, upgrade_t *upgr)
{
   if(Lith_GetPCVarInt(p, "lith_weapons_riflescope") &&
      p->weapon.cur->info->type == weapon_rifle &&
      p->riflefiremode == rifle_firemode_burst)
   {
      ACS_SetHudClipRect(40, 90, 240, 40);
      
      ACS_SetCameraToTexture(p->cameratid, "LITHCAM1", 34);
      DrawSpritePlain("LITHCAM1", hid_rifle_scope_cam, 0.1, 0.1, TICSECOND);
      
      DrawSpritePlain("lgfx/RifleScope.png", hid_rifle_scope_img, 40.1, 90.1, TICSECOND);
      
      ACS_SetHudClipRect(0, 0, 0, 0);
   }
}

// EOF

