// Copyright © 2016-2017 Graham Sanderson, all rights reserved.
#include "lith_upgrades_common.h"

#define UData UData_ReflexWetw(upgr)

#define CHARGE_MAX (35 * 0.8)

// Static Functions ----------------------------------------------------------|

//
// DOOOOODGE
//
[[__call("ScriptS")]]
static void DOOOOODGE(player_t *p)
{
   fixed vh = p->viewheight;

   for(int i = 0; i < 20; i++)
   {
      fixed mul = 1.0 - (ACS_Sin(i / 40.0) * 0.6);
      ACS_SetActorPropertyFixed(0, APROP_ViewHeight, vh * mul);
      ACS_Delay(1);
   }

   ACS_SetActorPropertyFixed(0, APROP_ViewHeight, vh);
}

// Extern Functions ----------------------------------------------------------|

//
// Activate
//
void Upgr_ReflexWetw_Activate(player_t *p, upgrade_t *upgr)
{
   p->speedmul += 0.3;
   UData.charge = CHARGE_MAX;
   UData.leaped = 0;
}

//
// Deactivate
//
void Upgr_ReflexWetw_Deactivate(player_t *p, upgrade_t *upgr)
{
   p->speedmul -= 0.3;
}

//
// Update
//
[[__call("ScriptS")]]
void Upgr_ReflexWetw_Update(player_t *p, upgrade_t *upgr)
{
   if(UData.charge < CHARGE_MAX)
      UData.charge++;

   if(p->frozen) return;

   fixed grounddist = p->z - p->floorz;

   if(UData.charge >= CHARGE_MAX)
   {
      if(grounddist == 0.0)
         UData.leaped = 0;

      upgrade_t *jet = p->getUpgr(UPGR_JetBooster);
      if(p->buttons & BT_SPEED &&
         (grounddist <= 16.0 || !jet || !jet->active ||
         UData_JetBooster(jet).discharged))
      {
         fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);

         ACS_PlaySound(0, "player/slide");
         p->setVel(p->velx + (ACS_Cos(angle) * 32.0), p->vely + (ACS_Sin(angle) * 32.0), 0, false, true);

         DOOOOODGE(p);

         UData.charge = 0;
      }
   }

   if(p->buttonPressed(BT_JUMP) &&
      !ACS_CheckInventory("Lith_RocketBooster") && !UData.leaped &&
      ((grounddist <= 16.0 && UData.charge < CHARGE_MAX) || grounddist > 16.0))
   {
      fixed angle = p->yaw - ACS_VectorAngle(p->forwardv, p->sidev);

      ACS_PlaySound(0, "player/doublejump");
      p->setVel(p->velx + (ACS_Cos(angle) * 4.0), p->vely + (ACS_Sin(angle) * 4.0), 12.0, false, true);

      UData.leaped = 1;
   }
}

// EOF

