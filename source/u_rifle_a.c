// ╭──────────────────────────────────────────────────────────────────────────╮
// │                                                                          │
// │             Distributed under the CC0 public domain license.             │
// │   By Alison G. Watson. Attribution is encouraged, though not required.   │
// │                See licenses/cc0.txt for more information.                │
// │                                                                          │
// ├──────────────────────────────────────────────────────────────────────────┤
// │                                                                          │
// │ Rifle_A upgrade.                                                         │
// │                                                                          │
// ╰──────────────────────────────────────────────────────────────────────────╯

#include "u_all.h"

void Upgr_Rifle_A_Deactivate(void) {
   pl.riflefiremode = 0;
}

void Upgr_Rifle_A_Render(void) {
   if(P_Wep_CurType() != weapon_m_rifle) return;

   if(CVarGetI(sc_weapons_riflescope) &&
      pl.riflefiremode == rifle_firemode_burst)
   {
      ACS_SetCameraToTexture(0, sp_LITHCAM1, 34);
      SetClip(40, 100, 240, 40);
      PrintSprite(sp_LITHCAM1, 160,0, 120,0);
      ClearClip();
      PrintSprite(sp_HUD_RifleScope, 160,0, 120,0);
   }

   PrintSprite(sp_HUD_H_W3, pl.hudrpos-105,2, 240,2);
   PrintSprite(sa_rifle_modes[rifle_firemode_max - pl.riflefiremode - 1], pl.hudrpos-105,2, 208 + (pl.riflefiremode * 16),2);
}

script_str ext("ACS") addr(OBJ "SwitchRifleFiremode")
void Z_SwitchRifleFiremode(void) {
   pl.riflefiremode = ++pl.riflefiremode % rifle_firemode_max;
   AmbientSound(ss_weapons_rifle_firemode, 1.0);
}

script_str ext("ACS") addr(OBJ "ResetRifleMode")
void Z_ResetRifleMode(void) {
   if(CVarGetI(sc_weapons_riflemodeclear)) {
      pl.riflefiremode = 0;
   }
}

/* EOF */
