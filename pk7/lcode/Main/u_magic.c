#include "lith_upgrades_common.h"

#define UData UData_Magic(upgr)


//----------------------------------------------------------------------------
// Extern Functions
//

//
// Update
//
[[__call("ScriptS")]]
void Upgr_Magic_Update(player_t *p, upgrade_t *upgr)
{
   fixed manaperc = ACS_CheckInventory("Lith_MagicAmmo") / (fixed)ACS_GetMaxInventory(0, "Lith_MagicAmmo");
   
   if(UData.manaperc < 1 && manaperc == 1)
      ACS_LocalAmbientSound("player/manafull", 127);
   
   UData.manaperc = manaperc;
   
   if(manaperc < 0.5 && ACS_Timer() % 5 == 0)
      ACS_GiveInventory("Lith_MagicAmmo", 1);
}

//
// Render
//
void Upgr_Magic_Render(player_t *p, upgrade_t *upgr)
{
   int hprc = ceilk(min(UData.manaperc,       0.5) * 2 * 33);
   int fprc = ceilk(max(UData.manaperc - 0.5, 0.0) * 2 * 33);
   
   DrawSpritePlain("lgfx/HUD_C/MagicIcon.png", hid_magicsymbol, 75.1, 199.2, TICSECOND);
   DrawSpritePlain("lgfx/HUD_C/BarVert.png",   hid_magicammobg, 67.1, 199.2, TICSECOND);
   
   ACS_SetHudClipRect(68, 198 - hprc, 5, hprc);
   DrawSpritePlain("lgfx/HUD_C/ManaBar1.png",  hid_magicammo1,  68.1, 198.2, TICSECOND);
   ACS_SetHudClipRect(0, 0, 0, 0);
   
   ACS_SetHudClipRect(68, 198 - fprc, 5, fprc);
   DrawSpritePlain("lgfx/HUD_C/ManaBar2.png",  hid_magicammo2,  68.1, 198.2, TICSECOND);
   ACS_SetHudClipRect(0, 0, 0, 0);
}

// EOF

