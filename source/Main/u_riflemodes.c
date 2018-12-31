// Copyright © 2016-2017 Alison Sanderson, all rights reserved.
#include "u_common.h"

// Extern Functions ----------------------------------------------------------|

script ext("ACS")
void Lith_SwitchRifleFiremode(void)
{
   withplayer(LocalPlayer)
   {
      p->riflefiremode = ++p->riflefiremode % rifle_firemode_max;
      ACS_LocalAmbientSound(ss_weapons_rifle_firemode, 127);
   }
}

script ext("ACS")
void Lith_ResetRifleMode()
{
   withplayer(LocalPlayer)
      if(p->getCVarI(sc_weapons_riflemodeclear))
         p->riflefiremode = 0;
}

stkcall
void Upgr_RifleModes_Deactivate(struct player *p, struct upgrade *upgr)
{
   p->riflefiremode = 0;
}

stkcall
void Upgr_RifleModes_Render(struct player *p, struct upgrade *upgr)
{
   if(p->weapontype != weapon_rifle) return;

   if(p->getCVarI(sc_weapons_riflescope) &&
      p->riflefiremode == rifle_firemode_burst)
   {
      SetClip(40, 108, 240, 40);

      ACS_SetCameraToTexture(p->cameratid, s_LITHCAM1, 34);
      PrintSprite(s_LITHCAM1, 0,1, 128,0);

      PrintSprite(sp_RifleScope, 40,1, 108,1);

      ClearClip();
   }

   PrintSprite(sp_HUD_H_W3, 215,2, 240,2);
   PrintSprite(StrParam(":HUD:H_W%u", (rifle_firemode_max - p->riflefiremode) + 3),
      215,2, 208 + (p->riflefiremode * 16),2);
}

// EOF
