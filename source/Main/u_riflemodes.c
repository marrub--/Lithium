// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

// Extern Functions ----------------------------------------------------------|

//
// Deactivate
//
stkcall
void Upgr_RifleModes_Deactivate(struct player *p, upgrade_t *upgr)
{
   p->riflefiremode = 0;
}

//
// Render
//
stkcall
void Upgr_RifleModes_Render(struct player *p, upgrade_t *upgr)
{
   if(p->getCVarI("lith_weapons_riflescope") &&
      p->weapontype == weapon_rifle &&
      p->riflefiremode == rifle_firemode_burst)
   {
      SetClip(40, 108, 240, 40);

      ACS_SetCameraToTexture(p->cameratid, "LITHCAM1", 34);
      PrintSprite("LITHCAM1", 0,1, 128,0);

      PrintSprite(":RifleScope", 40,1, 108,1);

      ClearClip();
   }
}

// EOF

