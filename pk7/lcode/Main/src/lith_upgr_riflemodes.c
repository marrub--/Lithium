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
// Update
//
[[__call("ScriptS")]]
void Upgr_RifleModes_Update(player_t *p, upgrade_t *upgr)
{
   if(ACS_GetUserCVar(p->number, "lith_weapons_riflescope") &&
      p->curweapon.info->type == weapon_rifle &&
      p->riflefiremode == rifle_firemode_burst)
   {
      ACS_Warp(p->cameratid, 0, 0, p->viewheight, 0,
               WARPF_NOCHECKPOSITION | WARPF_MOVEPTR |
               WARPF_COPYINTERPOLATION | WARPF_COPYPITCH);
      
      ACS_SetHudSize(320, 200);
      ACS_SetHudClipRect(40, 90, 240, 40);
      DrawSpritePlain("lgfx/RifleScope.png", hid_rifle_scope_img, 40.1, 90.1, TICSECOND);
      DrawSpritePlain("LITHCAM1", hid_rifle_scope_cam, 0.1, 0.1, TICSECOND);
      ACS_SetHudClipRect(0, 0, 0, 0);
   }
}

// EOF

