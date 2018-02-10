// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"
#include "lith_hud.h"

#define UData UData_HeadsUpDis3(upgr)

// Static Functions ----------------------------------------------------------|

//
// HUD_Health
//
static void HUD_Health(player_t *p, upgrade_t *upgr)
{
   PrintSprite("lgfx/HUD_I/HPAPBack.png", 0,1, 230,2);

   int health = (UData.healthi = lerpk(UData.healthi, p->health, 0.2)) + 0.5;
   PrintTextFmt("\C[Lith_Purple]%i", health);
   PrintText("LHUDFONT", CR_PURPLE, 21,1, 202,0);
}

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
void Upgr_HeadsUpDis3_Activate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = true;
}

//
// Disable
//
void Upgr_HeadsUpDis3_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->hudenabled = false;
}

//
// Render
//
void Upgr_HeadsUpDis3_Render(player_t *p, upgrade_t *upgr)
{
   Lith_HUD_Log(p, CR_LIGHTBLUE, 0, -15);
   Lith_HUD_KeyInd(p, 20, 20, false, 0.8);

   Lith_HUD_WeaponSlots(p, CR_DARKGRAY, CR_GRAY, CR_WHITE, "g", 323, 208);

   score_t score;

   if(p->score < 0x20000000000000LL)
   {
      #pragma GDCC FIXED_LITERAL OFF
      score = (UData.scorei = lerp(UData.scorei, p->score, 0.3)) + 0.5;
   }
   else
      score = p->score;

   Lith_HUD_Score(p, "%S \CnScore", score, "CHFONT", "[Lith_Purple]", 2,1, 3,1);

   // Status
   HUD_Health(p, upgr);
}

// EOF

